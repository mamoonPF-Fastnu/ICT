
// Experiment 1:Block scope & function scope 

function checkScope() {
    if (true) {
        var x = 10;
        let y = 20;
    }

    // Predicted:
    // x = 10
    // y = ReferenceError (let has block scope inside if statement block)

    console.log(x);
    console.log(y);

    // Actual:
    // 10
    // ReferenceError: y is not defined
}

checkScope();


// Experiment 2: Hoisting with var

// Predicted: undefined
console.log(hoistedVar);

var hoistedVar = 10;

// Actual: undefined


// Experiment 3: Hoisting with let

// Predicted:
// ReferenceError: Cannot access 'hoistedLet' before initialization

console.log(hoistedLet);

let hoistedLet = 20;

// Actual:
// ReferenceError: Cannot access 'hoistedLet'
// before initialization
