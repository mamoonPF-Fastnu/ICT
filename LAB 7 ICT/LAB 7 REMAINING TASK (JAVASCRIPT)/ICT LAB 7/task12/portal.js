function loginStudent() {
    const name = document.getElementById("loginName").value;
    const roll = document.getElementById("loginRoll").value;

    // Persist values across page navigation
    localStorage.setItem("studentName", name);
    localStorage.setItem("studentRoll", roll);

    window.location.href = "dashboard.html";
}

function loadDashboard() {
    const name = localStorage.getItem("studentName");
    const roll = localStorage.getItem("studentRoll");

    if (name) {
        document.getElementById("welcomeMsg").innerHTML = `Welcome, ${name}!`;
        document.getElementById("rollDisplay").innerHTML = `Roll Number: ${roll}`;
    }
}