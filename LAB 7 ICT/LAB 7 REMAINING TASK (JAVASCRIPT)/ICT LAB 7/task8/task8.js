function calculateBill() {
    const name = document.getElementById("prodName").value;
    // converting to number before input
    const price = Number(document.getElementById("prodPrice").value);
    const qty = Number(document.getElementById("prodQty").value);
    const total = price * qty;

    document.getElementById("billResult").innerHTML = `
        <p>Product Name: ${name}</p>
        <p>Price: Rs. ${price}</p>
        <p>Quantity: ${qty}</p>
        <p>Total Bill: Rs. ${total}</p>
    `;
}