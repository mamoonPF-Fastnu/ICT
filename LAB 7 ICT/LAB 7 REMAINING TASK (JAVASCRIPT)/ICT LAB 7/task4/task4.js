const student = {
    name: "Mamoon",
    rollNo: 101,
    marks: [85, 90, 78]
};

console.log("Student Name:", student.name);
console.log("Roll Number:", student["rollNo"]);

let key = "rollNo";
console.log("Roll Number using key:", student[key]);

student.marks[1] = 95;
student.grade = "A";
console.log("Updated Student Object:", student);

// Try to assign a completely new object
// student = { name: "Ali", rollNo: 102, marks: [70, 80, 90] };

// Error: TypeError: Assignment to constant variable.
//  Reason: A const variable cannot be reassigned to a new object.