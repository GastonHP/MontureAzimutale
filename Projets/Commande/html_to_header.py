import os

Import("env")

# 📂 Définition des chemins exacts
html_file = os.path.join(env.get("PROJECT_DIR"), "html", "index.html")
header_file = os.path.join(env.get("PROJECT_DIR"), "include", "html_autogen.h")


# 🛠️ Fonction de conversion (exécutée par PlatformIO s'il détecte un changement)
def generate_html_header(target, source, env):
    src_path = str(source[0])
    tgt_path = str(target[0])

    print(f"⚙️ [AUTO-GEN] Modification détectée sur {src_path} !")
    print(f"⚙️ [AUTO-GEN] Génération de {tgt_path}...")

    with open(src_path, "r", encoding="utf-8") as f:
        html_content = f.read()

    cpp_content = f"""// ⚠️ FICHIER GÉNÉRÉ AUTOMATIQUEMENT - NE PAS MODIFIER DIRECTEMENT
// Modifie plutôt le fichier /html/index.html

#ifndef HTML_AUTOGEN_H
#define HTML_AUTOGEN_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral({html_content})rawliteral";

#endif
"""

    # Sécurité pour créer le dossier include s'il manque
    os.makedirs(os.path.dirname(tgt_path), exist_ok=True)

    with open(tgt_path, "w", encoding="utf-8") as f:
        f.write(cpp_content)

    print("✅ [AUTO-GEN] Fichier mis à jour avec succès.")
    return None


# 🎯 LA MAGIE PLATFORMIO :
# On crée une règle de construction (Command) : "Pour fabriquer 'header_file' à partir de 'html_file', lance la fonction 'generate_html_header'"
gen_target = env.Command(header_file, html_file, generate_html_header)

# On indique à PlatformIO que la compilation de main.cpp dépend DIRECTEMENT de la création de ce header
# env.Depends("$BUILD_DIR/src/main.cpp.o", gen_target)