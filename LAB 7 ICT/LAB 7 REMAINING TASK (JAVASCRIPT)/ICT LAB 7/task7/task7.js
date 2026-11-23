function registerStudent() {
    const name = document.getElementById("stdName").value;
    const roll = document.getElementById("stdRoll").value;
    const dept = document.getElementById("stdDept").value;
    const sem = document.getElementById("stdSem").value;

    document.getElementById("output").innerHTML = `
        <p>Student Name: ${name}</p>
        <p>Roll Number: ${roll}</p>
        <p>Department: ${dept}</p>
        <p>Semester: ${sem}</p>
    `;
}