document.addEventListener("DOMContentLoaded", function() {
    const putForm = document.getElementById("putForm");

    if (putForm) {
        putForm.addEventListener("submit", function(event) {
            event.preventDefault(); // Oprește comportamentul normal al formularului

            const data = document.getElementById("putData").value;

            fetch("/put", {
                method: "PUT",
                headers: {
                    "Content-Type": "application/x-www-form-urlencoded",
                },
                body: "data=" + encodeURIComponent(data)
            })
            .then(response => response.text())
            .then(data => {
                console.log("Response:", data);
                // Poți adăuga cod pentru a afișa un mesaj de succes
                alert("Request PUT trimis cu succes!");
            })
            .catch((error) => {
                console.error("Error:", error);
                // Poți adăuga cod pentru a afișa un mesaj de eroare
                alert("Eroare la trimiterea requestului PUT.");
            });
        });
    }
});
