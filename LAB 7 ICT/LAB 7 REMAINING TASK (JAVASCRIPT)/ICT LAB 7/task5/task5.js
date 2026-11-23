//Experiment
const cart = ["Sharpner", "Pencil", "Keyboard"];
// Points to the same array reference
let backup = cart;

backup.push("USB Hub");
console.log("Cart:", cart);
console.log("Backup:", backup);

// Experiment 2: 
let a = 10;
// Copies the primitive number value
let b = a;
b = 50;
console.log("a:", a);
console.log("b:", b);

// Experiment 3: Independent Copy using Spread Operation
const independentBackup = [...cart];
independentBackup.push("Monitor");

console.log("Cart after spread push:", cart);
console.log("Independent backup:", independentBackup);