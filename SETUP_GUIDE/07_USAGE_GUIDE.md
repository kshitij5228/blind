# 📱 Daily Usage Guide Bhai | Roz Kaise Use Karein

**Device ko rozana ki zindagi mein kaise istemal karein**

---

## 🔋 Step 1: Device Ko Power On Karo

### Option A: Development Setup (USB cables)
1. Backend server start karo computer par
2. ESP32 Master ko USB cable se connect karo
3. ESP32-CAM ko USB cable se connect karo
4. Wait 10 seconds for WiFi connection
5. ✅ Ready to use

### Option B: Portable Setup (Power Bank)
1. Backend server must be running (computer ya cloud)
2. Power bank se both ESP32s ko power do
3. ESP32 Master automatically WiFi se connect hoga
4. ✅ Ready to use

**Status LED:**
- **Blinking**: WiFi se connect ho raha hai
- **2 Blinks**: System Ready hai
- **5 Blinks**: Error hua (device restart karo)

---

## 📸 Mode 1: Snapshot Mode (Quick Description)

### Kab Use Karein:
- Jaldi mein environment samajhna hai
- Koi specific object dhundhna hai
- Raaste mein obstacles check karna hai

### Kaise Use Karein:

1. **Camera ko target par point karo**
   - Object ke saamne 2-5 feet distance
   - Stable rakho (hilao mat)

2. **Button ko 1 second dabao aur chhodo**
   - Short press = Snapshot mode
   - LED ek baar blink karega

3. **Wait karo 3-5 seconds**
   - Device image capture karega
   - Backend ko bhejega
   - AI analyze karega

4. **Speaker se suno response**
   - Example responses:
     - "I see a wooden chair in front of you, approximately 3 feet away"
     - "There's a door on your right side, about 6 feet from you"
     - "A cup is on the table at 2 o'clock position"

### Use Cases:

**🏠 Ghar Mein:**
- "Where is my phone?"
- "What's in front of me?"
- "Is the door open or closed?"

**🚶 Bahar:**
- "Any obstacles ahead?"
- "What street sign is this?"
- "Is it safe to cross?"

**🍽️ Khana:**
- "What food is on my plate?"
- "Where is the spoon?"

---

## 💬 Mode 2: Conversation Mode (Questions Pucho)

### Kab Use Karein:
- Detailed jaankari chahiye
- Follow-up questions puchne hain
- Complex scenes samajhne hain

### Kaise Use Karein:

1. **Camera ko point karo**

2. **Button ko 3+ seconds dabaye rakho**
   - Long press = Conversation mode
   - LED continuously blink karega
   - Recording shuru ho jayegi

3. **Apna sawal bolo (mic ke paas)**
   - Clear aur loud bolo
   - Examples:
     - "What color is this shirt?"
     - "Can you read this label?"
     - "How many people are in front of me?"
     - "What's written on this sign?"

4. **Button chhodo**
   - Recording stop ho jayegi
   - Processing shuru hogi

5. **Wait karo 5-10 seconds**
   - Audio transcribe hoga
   - Image analyze hoga with your question
   - Answer generate hoga

6. **Speaker se suno detailed answer**

### Conversation Tips:

**✅ Ache Questions:**
- "What color is this?"
- "Can you describe the room?"
- "Where should I walk?"
- "Is there any text visible?"

**❌ Avoid:**
- Bahut lambe questions
- Background noise ke saath
- Multiple questions ek saath

---

## 🎯 Practical Scenarios

### Scenario 1: Morning Routine

```
1. Snapshot: "Where are my shoes?"
   → "I see brown shoes near the bed, on your left"

2. Snapshot: "What clothes are on the chair?"
   → "A blue shirt and black pants are folded on the chair"

3. Conversation: "What's the weather like outside?"
   → [Point camera to window]
   → "It appears to be sunny with clear skies"
```

### Scenario 2: Reading Labels/Text

```
1. Conversation: "Can you read this medicine label?"
   → [Hold medicine bottle in front of camera]
   → "The label says: Paracetamol 500mg, Take one tablet..."

2. Conversation: "What's the expiry date?"
   → [Keep holding bottle]
   → "The expiry date is March 2025"
```

### Scenario 3: Navigation Help

```
1. Snapshot: [Point forward]
   → "Clear path ahead, wall is 10 feet away"

2. Snapshot: [Point right]
   → "Doorway on your right, 4 feet from you"

3. Conversation: "Which room is this door?"
   → [Point at door sign]
   → "The sign says 'Bathroom'"
```

### Scenario 4: Finding Lost Items

```
1. Conversation: "Can you see my phone anywhere?"
   → [Pan camera across room]
   → "I can see a phone on the table at your 2 o'clock position"

2. Snapshot: [Point at table]
   → "Yes, a smartphone is on the right side of the table"
```

---

## ⚡ Power Management

### Battery Life (10000mAh Power Bank):

**Normal Use:**
- Snapshot mode only: ~12 hours
- Mixed usage: ~10 hours
- Heavy conversation mode: ~8 hours

### Power Saving Tips:

1. **Jab jarurat na ho tab band karo**
   - Unplug USB ya power bank switch off karo

2. **Snapshot mode zyada use karo**
   - Kam power consumption
   - Faster responses

3. **Unnecessary queries kam karo**
   - Plan your questions
   - Batch multiple items in one image

### Charging:

- Power bank ko regular phone charger se charge karo
- Charging time: 4-6 hours (10000mAh)
- Use karte hue charge kar sakte ho

---

## 🔧 Daily Maintenance

### Har Roz:
- [ ] Power bank charge level check karo
- [ ] Camera lens clean rakho (soft cloth se)
- [ ] Microphone area dust-free rakho

### Har Hafte:
- [ ] Saare connections check karo
- [ ] Loose wires tighten karo
- [ ] Speaker dust clean karo

### Mahine Mein Ek Baar:
- [ ] Backend server update karo (`git pull`)
- [ ] Firmware update check karo
- [ ] Full system test karo

---

## 🆘 Quick Troubleshooting

### "Device respond nahi kar raha"
1. Power check karo (LED on?)
2. WiFi check karo (router on?)
3. Device restart karo (unplug aur plug karo)

### "Audio response nahi aa raha"
1. Speaker volume check karo
2. Speaker connections check karo
3. Backend server running hai?

### "Poor quality responses mil rahe"
1. Camera lens clean karo
2. Better lighting mein use karo
3. Closer to object (2-3 feet)

### "WiFi baar baar disconnect ho raha"
1. Router ke paas aao
2. 2.4GHz WiFi use karo
3. Router restart karo

---

## 📞 Help Chahiye?

### Before asking for help:

1. ✅ Check [06_TESTING.md](06_TESTING.md) common problems
2. ✅ Device restart karo
3. ✅ Backend server logs check karo
4. ✅ Saare connections verify karo

### Contact:
- GitHub Issues: Bugs report karo
- Email: Support ke liye
- Community Forum: Tips aur tricks

---

## 🎓 Advanced Usage Tips

### Better Image Recognition:
- **Good lighting** bahut important hai
- **Stable hold** karo camera ko
- **2-5 feet distance** ideal hai
- **One object at a time** clearly visible

### Better Voice Recognition:
- **Clear pronunciation**
- **Background noise minimize** karo
- **Normal pace mein bolo**
- **1-2 feet from mic**

### Faster Responses:
- **Strong WiFi signal**
- **Backend fast computer par**
- **Peak hours avoid** karo (kam network traffic)

### Privacy:
- **Images stored nahi** hote (temporary processing only)
- **Audio transcribed** aur phir delete
- **Session data** temporary hai
- **API calls encrypted** hain

---

## ✅ Daily Usage Checklist

### Morning:
- [ ] Device power on karo
- [ ] WiFi connection check karo
- [ ] Ek snapshot se test karo

### During Day:
- [ ] Jarurat ke hisaab se use karo
- [ ] Camera lens clean rakho
- [ ] Power bank level monitor karo

### Evening:
- [ ] Power bank charge karo agar jarurat ho
- [ ] Loose connections check karo
- [ ] Device power off karo

---

## 🎯 Enjoy Your AI Vision Assistant Bhai!

Ab tum ready ho device ko daily life mein use karne ke liye! 

**Yaad Rakho:**
- Practice se perfect hoga
- Device seekhega tumhare usage patterns
- Feedback dete raho for improvements

**Mast Use Karo Bhai! 🎉**
