function calculateSalary() {
    const name = document.getElementById("empName").value;
    const basic = Number(document.getElementById("basicSal").value);
    const allowance = Number(document.getElementById("allowance").value);

    const grossSalary = basic + allowance;
    const category = grossSalary > 50000
        ? "High Salary Category"
        : "Normal Salary Category";

    document.getElementById("salaryOutput").innerHTML = `
        <p>Employee Name: ${name}</p>
        <p>Basic Salary: Rs. ${basic}</p>
        <p>Allowance: Rs. ${allowance}</p>
        <p>Gross Salary: Rs. ${grossSalary}</p>
        <p>Category: ${category}</p>
    `;
}