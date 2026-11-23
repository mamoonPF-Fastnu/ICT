function placeOrder() {
    const customer = document.getElementById("custName").value;
    const item = document.getElementById("foodItem").value;
    const price = Number(document.getElementById("foodPrice").value);
    const qty = Number(document.getElementById("foodQty").value);

    const totalPrice = price * qty;
    const discountMsg = totalPrice > 2000
        ? "You are eligible for a discount"
        : "No discount available";

    document.getElementById("orderSummary").innerHTML = `
        <p>Customer Name: ${customer}</p>
        <p>Food Item: ${item}</p>
        <p>Price: Rs. ${price}</p>
        <p>Quantity: ${qty}</p>
        <p>Total Price: Rs. ${totalPrice}</p>
        <p>${discountMsg}</p>
    `;
}