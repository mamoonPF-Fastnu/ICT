const tasks = [];

function addTask(task) {
    if (!task || task.trim() === "") {
        console.log("Error: Task cannot be empty.");
        return;
    }
    if (tasks.includes(task)) {
        console.log(`Error: "${task}" is already in the list.`);
        return;
    }
    tasks.push(task);
    console.log(`Added: "${task}"`);
}

function markDone(index) {
    if (index < 0 || index >= tasks.length) {
        console.log(`Error: Invalid index ${index}.`);
        return;
    }
    if (!tasks[index].startsWith("[Done] ")) {
        tasks[index] = "[Done] " + tasks[index];
    }
}

function updateTask(index, newText) {
    if (index < 0 || index >= tasks.length) {
        console.log(`Error: Invalid index ${index}.`);
        return;
    }
    if (!newText || newText.trim() === "") {
        console.log("Error: Updated task text cannot be empty.");
        return;
    }
    tasks[index] = newText;
}

function countPending() {
    let pending = 0;
    for (let i = 0; i < tasks.length; i++) {
        if (!tasks[i].startsWith("[Done] ")) {
            pending++;
        }
    }
    return pending;
}

function clearTasks() {
    // Empties the array in place without reassigning const tasks
    tasks.length = 0;
}

addTask("Finish ICT Lab 07");
addTask("Practice C Pointers");
addTask("Finish ICT Lab 07");
addTask("");
addTask("Solve Calculus Exercises");

markDone(0);
updateTask(1, "Practice C Arrays & Pointers");
updateTask(10, "Out of bounds");

console.log("Tasks:", tasks);
console.log("Pending tasks:", countPending());

clearTasks();
console.log("Cleared tasks:", tasks);