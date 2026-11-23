function checkEligibility() {
    const name = document.getElementById("applicantName").value;
    const total = Number(document.getElementById("totalMarks").value);
    const obtained = Number(document.getElementById("obtainedMarks").value);

    const percentage = (obtained / total) * 100;
    const status = percentage >= 50
        ? "Eligible for Admission"
        : "Not Eligible for Admission";

    document.getElementById("eligibilityOutput").innerHTML = `
        <p>Student Name: ${name}</p>
        <p>Percentage: ${percentage.toFixed(2)}%</p>
        <p>Status: ${status}</p>
    `;
}