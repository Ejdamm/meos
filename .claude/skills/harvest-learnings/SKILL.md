---
name: harvest-learnings
description: "Extract learnings from plan/progress.txt and feed them back into the source PRD (plan/prd-*.md). Closes the feedback loop between Ralph runs. Triggers on: harvest learnings, update prd with learnings, feed back learnings, sync progress to prd."
user-invocable: true
---

# Harvest Learnings

Extract codebase patterns and per-story learnings from `plan/progress.txt` and write them back into the source PRD (`plan/prd-*.md`), closing the feedback loop between Ralph iterations.

## Why This Exists

Ralph runs the entire migration from scratch each time. After each run, learnings are captured in `plan/progress.txt`. The project is then reset and Ralph runs again. Without this skill, those learnings are lost when progress.txt is deleted. This skill preserves them in the PRD itself — the one file that persists across runs.

---

## The Job

1. Read `plan/progress.txt` — extract **Codebase Patterns** and **per-story learnings**
2. Read `plan/prd.json` — identify the source PRD file and get story ID-to-title mappings
3. Find the source PRD (`plan/prd-*.md`) by matching the `project`/`description` from prd.json
4. Map progress.txt story IDs to PRD sections using prd.json as a bridge
5. Insert learnings into the PRD
6. Deduplicate against existing content

---

## Step 1: Parse progress.txt

### Extract Codebase Patterns

Look for the `## Codebase Patterns` section. Extract all bullet points (`- ...`). Stop at the first `---` separator.

**Example input:**
```
## Codebase Patterns
- Use `const object + type` pattern for enums (Vite 7 compatibility)
- Use `import type` for all TypeScript interfaces/types
- Use generic `DataTable` component for entity lists with sorting, filtering, and pagination.
```

### Extract Per-Story Learnings

Look for sections matching the pattern: `## <date> - <US-ID>: <title>` (e.g., `## 2026-03-09 - US-008: Classes page with CRUD`).

Within each section, find the `**Learnings for future iterations:**` subsection and extract all bullet points.

**Example input:**
```
## 2026-03-09 - US-008: Classes page with CRUD
- What was implemented:
  - Created `useCourses.ts` hook for fetching courses.
  ...
- **Learnings for future iterations:**
  - `useMutation` only takes one argument for `mutate`.
  - `FormSelect` values are always strings; remember to convert back to numbers if needed.
```

**Extract only:**
```
- `useMutation` only takes one argument for `mutate`.
- `FormSelect` values are always strings; remember to convert back to numbers if needed.
```

### Skip Timing-Only Entries

Sections that contain only timing info should be ignored. These match patterns like:
- `## US-005 (2m32s)`
- `## iteration-5 (5m29s)`
- `## US-016 — COMPLETED (9m26s)`

**Rule:** If a section heading has no subsequent bullet points or content paragraphs (only blank lines or the next heading), skip it.

---

## Step 2: Identify the Source PRD

1. Read `plan/prd.json`
2. Extract the `project` field (e.g., `"MeOS Web Frontend"`)
3. Find the matching `plan/prd-*.md` file — look for a PRD whose title/introduction mentions this project name
4. If only one `prd-*.md` file exists, use it directly

---

## Step 3: Map Story IDs via prd.json

The `/ralph` skill may renumber stories when converting from PRD to prd.json. For example, `US-007` in the PRD may become `US-001` in prd.json. Use the story **title** as the bridge:

1. For each story ID in progress.txt (e.g., `US-008`), look up its **title** in `prd.json`
2. Search the PRD for a section whose heading contains that title (or a close match)
3. That PRD section is the target for inserting learnings

**Example mapping:**
- progress.txt says: `US-008: Classes page with CRUD`
- prd.json US-008 has title: `"Classes page with CRUD"`
- PRD has: `### US-008: Web GUI — Competition Management` with sub-content about classes
- Target: the section in the PRD that covers classes

**Fallback:** If a story title from progress.txt maps to a sub-story in the PRD (e.g., the PRD groups multiple concerns under one US-ID), append the learnings to the parent story section.

---

## Step 4: Insert Learnings into the PRD

### 4a: Codebase Patterns Section

Insert a new section `## Codebase Patterns (from Previous Runs)` in the PRD, **immediately before** the `## User Stories` section.

**Format:**
```markdown
## Codebase Patterns (from Previous Runs)

These patterns were discovered during previous Ralph runs and should be followed:

- Use `const object + type` pattern for enums (Vite 7 compatibility)
- Use `import type` for all TypeScript interfaces/types
- Backend: XML API at `/meos`, Frontend: JSON REST API at `/api/v1`
```

**If the section already exists**, merge new patterns into it (see deduplication rules below).

### 4b: Per-Story Learnings

For each matched user story in the PRD, add a `**Learnings from Previous Runs:**` block **after** the existing `**Implementation Notes:**` section (or after `**Acceptance Criteria:**` if there are no Implementation Notes).

**Format:**
```markdown
### US-008: Web GUI — Competition Management

**Description:** As a competition organizer, I want to...

**Acceptance Criteria:**
- [ ] CRUD interface for classes...

**Implementation Notes:**
- Each entity type maps to a tab/route
- Table views should support sorting...

**Learnings from Previous Runs:**
- `useMutation` only takes one argument for `mutate`. Options must be passed to `useMutation` hook itself.
- `FormSelect` values are always strings; remember to convert back to numbers if needed for the API.
- `DataTable` is a powerful generic component but needs `isLoading` state to avoid flashing "No data" while fetching.
```

---

## Step 5: Deduplication Rules

Before adding any learning or pattern, check if it (or something semantically equivalent) already exists in the target location.

### For Codebase Patterns:
- Compare each new pattern against existing patterns in `## Codebase Patterns (from Previous Runs)`
- **Skip** if the same tool/component/concept is already mentioned with the same advice
- **Update** if the new version has more detail (replace the old one)

### For Per-Story Learnings:
- Compare each new learning against:
  1. Existing `**Learnings from Previous Runs:**` entries for that story
  2. Existing `**Implementation Notes:**` entries for that story
- **Skip** if the learning duplicates an Implementation Note (the PRD author already knows this)
- **Skip** if the learning is already in Learnings from Previous Runs
- **Update** if the new version has more detail

### Semantic comparison (not exact string match):
- `"FormSelect values are strings"` duplicates `"FormSelect values are always strings; convert to numbers"`
- `"Use DataTable for lists"` duplicates `"Use generic DataTable component for entity lists"`

---

## Example: Before and After

### Before (prd-web-frontend.md excerpt):

```markdown
## User Stories

### US-008: Web GUI — Competition Management

**Description:** As a competition organizer, I want to create and configure competitions...

**Acceptance Criteria:**
- [ ] CRUD interface for classes (name, course assignment, start method)
- [ ] CRUD interface for courses (name, length, controls)
...

**Implementation Notes:**
- Each entity type maps to a tab/route
- Table views should support sorting, filtering, and inline editing
```

### After:

```markdown
## Codebase Patterns (from Previous Runs)

These patterns were discovered during previous Ralph runs and should be followed:

- Use `const object + type` pattern for enums (Vite 7 compatibility)
- Use `import type` for all TypeScript interfaces/types
- Backend: XML API at `/meos`, Frontend: JSON REST API at `/api/v1`
- Use `NavLink` for active route highlighting
- Tailwind 4 for styling (CSS-first approach)
- Use generic `DataTable` component for entity lists with sorting, filtering, and pagination.
- Use `zod` for form validation and `react-hook-form` for form management.
- Reuse standard form components (`FormField`, `FormInput`, `FormSelect`, `SearchableSelect`) for consistent styling and validation.
- Use `size` property on `FormDialog` ('sm', 'md', 'lg', 'xl') to handle complex forms with varying width requirements.
- `DataTable` supports row selection with `enableSelection` prop.

## User Stories

### US-008: Web GUI — Competition Management

**Description:** As a competition organizer, I want to create and configure competitions...

**Acceptance Criteria:**
- [ ] CRUD interface for classes (name, course assignment, start method)
- [ ] CRUD interface for courses (name, length, controls)
...

**Implementation Notes:**
- Each entity type maps to a tab/route
- Table views should support sorting, filtering, and inline editing

**Learnings from Previous Runs:**
- `useMutation` only takes one argument for `mutate`. Options must be passed to `useMutation` hook itself, or handled via `async/await` in the caller.
- `z.coerce.number()` might cause type inference issues with `zodResolver`; sometimes `valueAsNumber: true` in `register` with `z.number()` is cleaner.
- `FormSelect` values are always strings; remember to convert back to numbers if needed for the API.
- `DataTable` is a powerful generic component but needs `isLoading` state to avoid flashing "No data" while fetching.
- `FormDialog` needed more flexibility for width; added a `size` prop for complex editors.
- `ControlSequenceBuilder` uses `SearchableSelect` for adding items, which works well for large lists.
```

---

## Checklist

Before saving the updated PRD:

- [ ] Read `plan/progress.txt` — extracted Codebase Patterns and per-story learnings
- [ ] Read `plan/prd.json` — identified source PRD and story mappings
- [ ] Skipped timing-only entries (no learnings content)
- [ ] Inserted `## Codebase Patterns (from Previous Runs)` before `## User Stories`
- [ ] Inserted `**Learnings from Previous Runs:**` per matched story
- [ ] Deduplicated against existing Implementation Notes and previous learnings
- [ ] Did NOT modify acceptance criteria, implementation notes, or any other PRD content
- [ ] Reported which stories had learnings added and which had no match
