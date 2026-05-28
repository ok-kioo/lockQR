const form = document.querySelector(".login-form");
const button = document.querySelector(".btn-primary");
const message = document.querySelector("#message");

function showMessage(text, type) {
  message.textContent = text;
  message.className = "form-message " + type;
}

form.addEventListener("submit", async (event) => {
  event.preventDefault();

  const formData = new FormData(form);
  const body = new URLSearchParams(formData);

  button.disabled = true;
  button.textContent = "Verificando...";
  showMessage("", "");

  try {
    const response = await fetch("/api/login", {
      method: "POST",
      headers: {
        "Content-Type": "application/x-www-form-urlencoded"
      },
      body
    });

    const data = await response.json();

    if (data.ok) {
      showMessage(data.message, "success");
      button.textContent = "Acesso liberado";
      form.reset();

      setTimeout(() => {
        button.textContent = "Entrar";
        button.disabled = false;
      }, 2500);

      return;
    }

    showMessage(data.message, "error");
    button.textContent = "Entrar";
    button.disabled = false;

  } catch (error) {
    showMessage("Erro ao conectar com o ESP32.", "error");
    button.textContent = "Entrar";
    button.disabled = false;
  }
});