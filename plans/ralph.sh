set -e

# 1. Define the prompt clearly outside the loop to avoid escaping hell
PROMPT="1. CONTEXT LOAD: Read 'plans/progress.txt' FIRST. If the last entry was a FAILURE, do not attempt the exact same solution again. \
2. TASK SELECTION: Identify the single most critical task. Prioritize blocking dependencies first. \
3. SCOPE: Implement the solution. RESTRICTION: You may read other files for context, but only modify files strictly necessary for this task. \
4. TESTING: NEVER run the full test suite. Run tests ONLY for the target file. \
5. LOGGING: \
   - If success: done:true in PRD and append summary to 'plans/progress.txt'. Make a git commit. \
   - If failure: Append 'FAILED: <brief_reason>' to 'plans/progress.txt' to warn the next agent. \
6. COMPLETION LOGIC: \
   - If implemented AND passed local tests: output <promise>COMPLETE</promise> \
   - If tests FAILED and you cannot fix it: output <promise>FAILED</promise> \
   - If NO tasks remain and PRD is done: output <promise>ALL_COMPLETE</promise>"

if [ -z "$1" ]; then
	echo "Usage: $0 iterations"
	exit 1
fi

for ((i = 1; i <= $1; i++)); do
	echo "=============================="
	echo "Iteration $i of $1"
	echo "=============================="

	# Capture output
	result=$(copilot --allow-all -p "@plans/prd.json @plans/progress.txt $PROMPT")

	# Print the AI's thought process to the console so you can watch
	echo "$result"

	# --- Logic Gates ---

	if [[ "$result" == *"<promise>ALL_COMPLETE</promise>"* ]]; then
		echo "✅ All tasks marked as done! Exiting."
		exit 0
	fi

	if [[ "$result" == *"<promise>COMPLETE</promise>"* ]]; then
		echo "✅ Task finished successfully. Continuing to next task..."
		continue
	fi

	if [[ "$result" == *"<promise>FAILED</promise>"* ]]; then
		echo "❌ Task failed. The agent logged the error to progress.txt."
		echo "Pausing for 5 seconds before retrying (the next agent will see the failure log)..."
		sleep 5
		continue
	fi

	# If no promise was returned, the LLM might be confused or just chatting.
	echo "⚠️  No valid completion tag found. Continuing loop..."
done

echo ""
echo "🛑 Limit reached: This was the last iteration ($1). Stopping execution."
