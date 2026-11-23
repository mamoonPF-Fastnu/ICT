function issueBook() {
    const student = document.getElementById("libStudent").value;
    const title = document.getElementById("bookTitle").value;
    const id = document.getElementById("bookId").value;
    const days = Number(document.getElementById("numDays").value);

    const status = days > 14
        ? "Please return the book on time"
        : "Book issued successfully";

    document.getElementById("libraryOutput").innerHTML = `
        <p>Student Name: ${student}</p>
        <p>Book Title: ${title}</p>
        <p>Book ID: ${id}</p>
        <p>Number of Days: ${days}</p>
        <p>Status: ${status}</p>
    `;
}