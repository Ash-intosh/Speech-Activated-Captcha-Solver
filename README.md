## Speech-Activated-Captcha-Solver

# Captcha Solver Using Voice Command

This project provides a voice-based solution for solving mathematical captchas using a backend server powered by Node.js and an HMM (Hidden Markov Model) predictor.

---

## Table of Contents
1. [How to Run the Code](#how-to-run-the-code)
2. [Folder Structure](#folder-structure)
3. [How It Works](#how-it-works)
4. [Dependencies](#dependencies)
5. [Notes](#notes)

---

## How to Run the Code

Follow these steps to set up and run the captcha solver:

1. **Ensure File Structure**: Keep all files and folders in the same directory structure as provided.

2. **Install Node.js**: 
   - For macOS:  
     ```bash
     brew install node
     ```
   - For other operating systems, download and install Node.js from [https://nodejs.org/](https://nodejs.org/).

3. **Install Dependencies**:  
   Install the required dependencies by running:
   ```bash
   npm install
   ```
   Make sure the dependencies listed in the `requirement.txt` file are installed.

4. **Start the Backend Server**:  
   Run the server file using Node.js:
   ```bash
   node server.js
   ```

5. **Access the Webpage**:  
   Open a Chrome browser and go to the following URL:  
   [http://localhost:3000](http://localhost:3000)

6. Your captcha solver is now ready to use!

---

## Folder Structure

```
CaptchaSolver/
│
├── AVG_Model_HMM/         # Contains average models created during HMM training
├── Codebook/              # Contains the Codebook with 32 vectors
├── Initial_Model/         # Contains the initial inertia model (uses PI matrix)
├── public/                # Contains index.html and welcome homepage files
│   ├── index.html
│   └── welcome.html
├── hmm.cpp                # Contains the HMM predictor code
├── hmm                    # HMM binary file for prediction
├── server.js              # Backend server logic and processing
├── requirement.txt        # List of dependencies
└── README.md              # Documentation file
```

---

## How It Works

1. **Mathematical Captcha**:  
   The webpage displays a captcha in the form of a simple mathematical equation.

2. **Voice Input**:  
   - The user clicks the **"Recording"** button and verbally says the answer to the captcha.
   - The voice input is recorded and sent to the backend server.

3. **Backend Processing**:  
   - The backend server processes the audio input.
   - The HMM model (using the predictor) analyzes the audio and predicts the word.

4. **Validation**:  
   - If the predicted answer matches the correct solution, the user is taken to the next page.

---

## Dependencies

Ensure the following are installed:

- **Node.js** (v14 or higher)
- **npm** (Node Package Manager)
- Dependencies listed in `requirement.txt`

To install dependencies, run:  
```bash
npm install
```

---

## Notes

1. If any changes are made in the `hmm` folder (e.g., modifying `hmm.cpp` or retraining the model), recompile the code to create the binary file:
   - Save the binary file with the same name `hmm`.

2. Use **Google Chrome** for the best experience.

---

## Demo Video

[Download and watch the demo video](Ash-intosh/Speech-Activated-Captcha-Solver/CAPTCHA-SOLVER.mp4)


Happy Captcha Solving! 🎤✨