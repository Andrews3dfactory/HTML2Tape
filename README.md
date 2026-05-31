# HTML2Tape 🚀  
A fast, WebSerial‑powered label input tool for the CrunchLabs Hack Pack Label Maker.  
Designed by Jacob Ehli • Forge Street Creations

HTML2Tape replaces the slow joystick input on the Hack Pack Label Maker with a clean, poppy, space‑themed UI that lets you type labels directly from your keyboard and send them instantly to the Arduino.

This project keeps the original Hack Pack firmware behavior — no mods required.  
Just connect, type, and print.

---

## ✨ Features
- WebSerial connection to the Hack Pack Label Maker  
- Type labels using your keyboard  
- One‑click “Connect” and “Print”  
- Animated starfield background  
- Forge Street Creations poppy UI theme  
- Zero firmware changes needed  

---

## 🛠 Requirements
- A Chromium‑based browser (Chrome, Edge, Brave, etc.)  
- Hack Pack Label Maker (Mark Rober / CrunchLabs)  
- Arduino firmware that accepts:
  - `CONNECTED`
  - `PRINT:<text>`

---

## 🚀 How to Use
1. Open `index.html` in a supported browser  
2. Click **Connect**  
3. Select your Arduino device  
4. Type your label  
5. Click **Print**  
6. Watch the robot do its thing  

---

## 📁 Project Structure

```
HTML2Tape/
│
├── index.html        # Main UI + WebSerial connection + starfield animation
├── README.md         # Project documentation, usage, features, credits
├── LICENSE.md        # MIT License for open-source distribution
└── HTML2Tape.ino     # Arduino firmware for the Hack Pack Label Maker
```


---

## 🧡 Credits
Created by **Andrew Ehli**  
Forge Street Creations — “Father & Son. Laser Forged.”

Inspired by the CrunchLabs Hack Pack Label Maker.

---

## 📜 License
This project is licensed under the MIT License.  
See `LICENSE.md` for details.
