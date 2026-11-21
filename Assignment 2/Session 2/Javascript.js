const readline = require("readline");

let tasks = [];

const r1=readline.createInterface({
    Input:ProcessingInstruction.stdin,output:process.srdout
});

function addTask(){
    r1.question("Ente the Task:",function(task){
        if(task.trim() !== ""){
            task.push(task);
            console.log("Task added successfully!");
        }else{
            console.log("Task cannot be empty!");
        }
        showMenu();
    });
}

function removeTask() {
    displayTasks();

    if(tasks.length==0){
        showMenu();
        return;
    }
    r1.question("Enter the task number to remove ",function(index){
        let num = parseInt(index);

        if(num>=1 && num<= tasks.length){
            tasks.splice(num-1, 1);
            console.log("Task Removed successfully");
        }else{
            console.log("Invalid Task number");
        }
        showMenu();
    });
}

function displayTasks() {
    if(tasks.length ===0) {
        console.log("Your to-do list is empty");
    }else {
        console.log("\n Your Tasks");
        for(let i=0;i<tasks.length;i++){
            console.log((i+1)+ "."+ tasks[i]);
        }
    }

}

function clearTask(){
    tasks = [];
    console.log("All tasks hvae been cleared");
}

function showMenu(){
    console.log("\n TO-DO LIST  ");
    console.log("1. Add Tasks");
    console.log("2. Remove Tasks");
    console.log("3. View Tasks");
    console.log("4. Clear Tasks");
    console.log("5. Exit");

    r1.question("Enter your choice: ",function(choice){
        switch(choice){
            case "1":
            addTask()
            break;

            case "2":
                removeTask();
                break;
            
            case "3":
                displayTasks();
                showMenu();
                break;

            case "4":
                clearTasks();
                showMenu();
                break;
            case "5":
                console.log("Invalid Choice,try again :");
                showMenu();
        }
    });
}
showMenu();