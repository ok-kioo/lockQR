#ifndef WEB_CONTENT_H
#define WEB_CONTENT_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>LockQR</title>
  <link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@24,400,0,0&icon_names=key" />
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <main class="login-container">

    <section class="login-hero">
      <span class="material-symbols-outlined hero-icon-desktop">key</span>

      <div class="hero-brand-mobile">
        <span class="material-symbols-outlined hero-icon-mobile">key</span>
        <h2 class="hero-logo-mobile">Lock<span class="text-highlight-light">QR</span></h2>
      </div>

      <div class="hero-content">
        <h3 class="hero-subtitle">Desbloqueie facilmente</h3>
        <h2 class="hero-title">Sua segurança na palma da mão, com acesso rápido e inteligente</h2>
      </div>
    </section>

    <section class="login-form-wrapper">
      <header class="form-header">
        <h1 class="form-logo">Lock<span class="text-highlight-primary">QR</span></h1>
        <h2 class="form-title">Bem-vindo de volta!</h2>
        <p class="form-subtitle">Confirme sua identidade para liberar o acesso</p>
      </header>

      <form class="login-form">
        <div class="form-group">
          <label for="user">Usuário</label>
          <input id="user" type="text" name="user" placeholder="Digite seu usuário" required>
        </div>

        <div class="form-group">
          <label for="senha">Senha</label>
          <input id="senha" type="password" name="senha" placeholder="Digite sua senha" required>
        </div>

        <button type="submit" class="btn-primary">Entrar</button>
      </form>
    </section>
  </main>

  <script type="module" src="app.js"></script>
</body>
</html>
)rawliteral";

const char STYLE_CSS[] PROGMEM = R"rawliteral(
:root {
  /* Paleta de Cores */
  --color-primary: #f9763d;
  --color-primary-hover: #f5672b;
  --color-text-dark: #222222;
  --color-text-muted: #777777;
  --color-text-light: #aaaaaa;
  --color-border: #b8b8b8;
  --color-bg-white: #ffffff;
  --color-logo-gray: #827b78;

  /* Cores do Degradê */
  --grad-base: #f5b99f;
  --grad-1: #f9763d;
  --grad-2: #ffb68c;
  --grad-3: #fcd2c8;
  --grad-4: #f6ece7;

  /* Tipografia */
  --font-main: Verdana, Geneva, Tahoma, sans-serif;
}

* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: var(--font-main);
  color: var(--color-text-dark);
  background-color: var(--color-bg-white);
  overflow-x: hidden;
}

/* Utilitários de Texto */
.text-highlight-primary {
  color: var(--color-primary);
}

.text-highlight-light {
  color: #ffece4;
}

.login-container {
  min-height: 100vh;
  display: flex;
  gap: 4vw;
  padding: 2.5vh 1vw;
}

.login-hero {
  width: 50vw;
  min-height: 95vh;
  border-radius: 1.4rem;
  padding: 2rem;
  position: relative;
  
  background-color: var(--grad-base);
  background-image: 
    radial-gradient(circle at 85% 90%, var(--grad-1) 0%, transparent 65%),
    radial-gradient(circle at 15% 95%, var(--grad-2) 0%, transparent 60%),
    radial-gradient(circle at 85% 10%, var(--grad-3) 0%, transparent 55%),
    radial-gradient(circle at 10% 10%, var(--grad-4) 0%, transparent 65%);
    
  box-shadow: 0 0.6rem 2rem rgba(0, 0, 0, 0.05);
  border: 0.25rem solid var(--color-bg-white);
}

.hero-icon-desktop {
  font-size: clamp(2.8rem, 5vw, 4rem);
  margin: 1vh 0 0 1vw;
}

.hero-brand-mobile {
  display: none;
}

.hero-content {
  position: absolute;
  left: 2rem;
  right: 2rem;
  bottom: 2rem;
  font-weight: bold;
}

.hero-subtitle {
  font-size: clamp(1.2rem, 2vw, 1.8rem);
  font-weight: normal;
  margin-bottom: 0.5rem;
}

.hero-title {
  font-size: clamp(2rem, 4vw, 3rem);
  line-height: 1.15;
  margin-bottom: 3rem;
}

.login-form-wrapper {
  flex: 1;
  min-height: 95vh;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
}

.form-header {
  text-align: center;
  width: 85%;
  max-width: 34rem;
  margin-bottom: 5vh;
}

.form-logo {
  font-size: 3rem;
  font-weight: 800;
  color: var(--color-logo-gray);
  margin-bottom: 5vh;
  letter-spacing: -0.04em;
}

.form-title {
  font-size: clamp(2.2rem, 4vw, 3rem);
  margin-bottom: 1vh;
  line-height: 1.1;
}

.form-subtitle {
  font-size: clamp(1rem, 2vw, 1.1rem);
  font-weight: 600;
  line-height: 1.3;
  max-width: 28rem;
  margin: 0 auto;
}

/* Elementos do Formulário */
.login-form {
  width: 85%;
  max-width: 32rem;
  display: flex;
  flex-direction: column;
  gap: 2rem;
}

.form-group {
  display: flex;
  flex-direction: column;
}

.form-group label {
  font-size: 1.1rem;
  color: var(--color-text-muted);
  font-weight: 600;
  margin-bottom: 0.4rem;
}

.form-group input {
  width: 100%;
  padding: 0.5rem 0;
  background: transparent;
  border: none;
  border-bottom: 0.08rem solid var(--color-border);
  outline: none;
  font-size: 1rem;
  font-family: inherit;
  color: var(--color-text-dark);
  transition: border-color 0.3s ease;
}

.form-group input::placeholder {
  color: var(--color-text-light);
}

.form-group input:focus {
  border-bottom-color: var(--color-primary);
}

.btn-primary {
  width: 100%;
  height: 3.5rem;
  margin-top: 3vh;
  
  background-color: var(--color-primary);
  color: var(--color-bg-white);
  
  border: none;
  border-radius: 0.9rem;
  cursor: pointer;
  
  font-size: 1.2rem;
  font-weight: bold;
  font-family: inherit;
  
  display: flex;
  justify-content: center;
  align-items: center;
  
  box-shadow: 0 0.5rem 1.2rem rgba(249, 118, 61, 0.25);
  transition: background-color 0.3s ease, transform 0.1s ease;
}

.btn-primary:hover {
  background-color: var(--color-primary-hover);
}

.btn-primary:active {
  transform: scale(0.98);
}

@media (max-width: 55rem) {
  .login-container {
    min-height: 100svh;
    flex-direction: column;
    gap: 4vh;
    padding: 2vh 5vw 4vh;
  }

  /* Ajustes do Hero (Card Superior) */
  .login-hero {
    width: 100%;
    min-height: 5.5rem;
    padding: 1.3rem;
    display: flex;
    align-items: center;
    justify-content: center;
  }

  .hero-icon-desktop,
  .hero-content {
    display: none;
  }

  .hero-brand-mobile {
    width: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 0.7rem;
  }

  .hero-icon-mobile {
    font-size: 1.6rem;
    color: var(--color-text-dark);
  }

  .hero-logo-mobile {
    font-size: 1.6rem;
    font-weight: 800;
    color: var(--color-text-dark);
    line-height: 1;
  }

  /* Ajustes do Formulário */
  .login-form-wrapper {
    min-height: auto;
    justify-content: flex-start;
    padding-bottom: 4vh;
  }

  .form-header {
    width: 90%;
    margin-bottom: 4vh;
  }

  .form-logo {
    display: none;
  }

  .form-title {
    font-size: 1.8rem;
  }

  .form-subtitle {
    font-size: 0.9rem;
    line-height: 1.25;
  }

  .login-form {
    width: 86%;
    gap: 1.8rem;
  }

  .form-group label {
    font-size: 1rem;
  }

  .form-group input {
    font-size: 0.95rem;
  }

  .btn-primary {
    height: 3.4rem;
    font-size: 1.1rem;
    margin-top: 2vh;
  }
}

/* Celulares pequenos (Até 560px / 35rem) */
@media (max-width: 35rem) {
  .login-hero {
    min-height: 5rem;
  }

  .hero-icon-mobile,
  .hero-logo-mobile {
    font-size: 1.35rem;
  }

  .form-title {
    font-size: 1.65rem;
  }

  .form-subtitle {
    font-size: 0.82rem;
  }

  .login-form {
    gap: 1.5rem;
  }
}
)rawliteral";

const char SCRIPT_JS[] PROGMEM = R"rawliteral(
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
)rawliteral";

#endif