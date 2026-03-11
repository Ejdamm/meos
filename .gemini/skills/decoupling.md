# Decoupling Domain from UI

This skill describes the pattern for decoupling domain model classes (like `oEvent`) from UI implementations (like `Tab*` classes) in the MeOS codebase.

## Pattern

1.  **Identify UI Dependency:** Find direct calls from domain code (`oEvent.cpp`) to UI classes (`TabList::baseButtons`, etc.).
2.  **Add Callback Member:** Add a `std::function` member to the domain class in its header (`oEvent.h`).
    ```cpp
    #include <functional>
    ...
    std::function<int(gdioutput&, int, bool)> baseButtonsCallback;
    ```
3.  **Add Setter:** Add a public setter for the callback.
    ```cpp
    void setBaseButtonsCallback(std::function<int(gdioutput&, int, bool)> cb) { baseButtonsCallback = cb; }
    ```
4.  **Replace Direct Call:** In the domain `.cpp` file, remove the UI header and replace the direct call with the callback (checked for validity).
    ```cpp
    // Remove #include "TabList.h"
    ...
    if (baseButtonsCallback) baseButtonsCallback(gdi, 1, false);
    ```
5.  **Register Callback:** In the application entry point or composition root (`meos.cpp`), register the actual UI method as the callback after the domain object is initialized.
    ```cpp
    // In meos.cpp
    gEvent = new oEvent(*gdi_main);
    gEvent->setBaseButtonsCallback(TabList::baseButtons);
    ```

## Benefits

-   **Testability:** Domain classes can be tested without UI dependencies by providing mock callbacks.
-   **Portability:** Domain code can be moved to a standalone library that doesn't know about the GUI.
-   **Maintainability:** Reduces circular dependencies and clarifies the boundary between domain and UI.

## Gotchas

-   Ensure the callback signature matches the original method signature.
-   Always check if the callback is set before calling it, unless you can guarantee it's always set.
-   Lambdas can be used in the registration step if the UI method needs extra context (like `gdi_main`).
