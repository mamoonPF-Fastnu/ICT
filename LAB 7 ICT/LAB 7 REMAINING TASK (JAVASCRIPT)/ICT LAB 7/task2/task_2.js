const city = "Karachi";
const age = 18;
const nationalPopulation = 80000000000000n;
const isHosteller = false;
let futureCourse;
const emptyValue = null;
const uniqueId = Symbol("studentID");

console.log(`city = ${city} | type = ${typeof city}`);
console.log(`age = ${age} | type = ${typeof age}`);
console.log(`nationalPopulation = ${nationalPopulation} | type = ${typeof nationalPopulation}`);
console.log(`isHosteller = ${isHosteller} | type = ${typeof isHosteller}`);
console.log(`futureCourse = ${futureCourse} | type = ${typeof futureCourse}`);
console.log(`emptyValue = ${emptyValue} | type = ${typeof emptyValue}`);
// Converting symbol explicitly into a string literal for string output
console.log(`uniqueId = ${String(uniqueId)} | type = ${typeof uniqueId}`);

// typeof "null" returns "object" because of JS quirk but kept for backwards compatibility