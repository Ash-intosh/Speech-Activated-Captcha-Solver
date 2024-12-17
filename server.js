const express = require('express');
const multer = require('multer');
const fs = require('fs');
const path = require('path');
const ffmpeg = require('fluent-ffmpeg');
const { exec } = require('child_process');
const wav = require('wav');

const app = express();

// Set up storage for uploaded files
const upload = multer({ dest: 'uploads/' });

// Serve static files (frontend)
app.use(express.static('public'));
app.use(express.json()); // For parsing JSON bodies
app.use(express.urlencoded({ extended: true })); // For parsing form data

// Endpoint to handle audio file upload
app.post('/upload', upload.single('audio'), (req, res) => {
    const uploadedFilePath = req.file.path;
    const outputWavFilePath = path.join(__dirname, 'uploads', 'output.wav');
    const outputTxtFilePath = path.join(__dirname, 'audio_data.txt');
    const captchaEquation = JSON.parse(req.body.captchaEquation); // Get the CAPTCHA equation from the frontend

    console.log('Received audio file:', uploadedFilePath);
    console.log('Generated CAPTCHA Equation:', captchaEquation.answer);

    // Convert the uploaded audio (WebM or OGG) to WAV
    convertToWav(uploadedFilePath, outputWavFilePath, (err) => {
        if (err) {
            return res.status(500).json({ message: 'Error converting audio file to WAV' });
        }

        // Process the WAV file to extract data
        processWavFile(outputWavFilePath, outputTxtFilePath, (err) => {
            if (err) {
                return res.status(500).json({ message: 'Error processing WAV file' });
            }

            // Trigger C++ code to compute CAPTCHA answer
            exec(`./hmm ${outputTxtFilePath}`, (err, stdout, stderr) => {
                if (err) {
                    return res.status(500).json({ message: 'Error processing audio with C++ program' });
                }

                // Processed result from C++ (output of CAPTCHA computation)
                console.log(`Computed stdout: ${stdout}`);
                const computedAnswer = parseInt(stdout.trim(), 10);
                console.log(`Computed CAPTCHA Answer from C++: ${computedAnswer}`);

                // Compare the computed answer from the C++ program to the equation result
                if (computedAnswer === captchaEquation.answer) {
                    // If the C++ result matches the correct CAPTCHA answer
                    return res.json({ success: true, message: 'Captcha solved! Redirecting...' });
                } else {
                    return res.json({ success: false, message: 'Incorrect CAPTCHA solution from audio.' });
                }
            });
        });
    });
});

// Convert audio file to WAV using FFmpeg
function convertToWav(inputFilePath, outputFilePath, callback) {
    ffmpeg(inputFilePath)
        .audioCodec('pcm_s16le')  // Convert to 16-bit signed PCM
        .audioFilters('aformat=s16:16000')  // 16-bit, 44.1 kHz
        .format('wav')
        .on('end', () => {
            console.log(`Audio converted to WAV: ${outputFilePath}`);
            callback(null);
        })
        .on('error', (err) => {
            console.error('Error during conversion:', err);
            callback(err);
        })
        .save(outputFilePath);
}

// Function to process WAV file
function processWavFile(inputFilePath, outputFilePath, callback) {
    const file = fs.createReadStream(inputFilePath);
    const reader = new wav.Reader();
    let formatData = null;
    const audioData = [];

    reader.on('format', function (format) {
        formatData = format;
    });

    reader.on('data', function (chunk) {
        audioData.push(chunk);
    });

    reader.on('end', function () {
        const fullAudioData = Buffer.concat(audioData);
        const writeStream = fs.createWriteStream(outputFilePath);

        for (let i = 0; i < fullAudioData.length; i += 2) {
            const sample = fullAudioData.readInt16LE(i);
            writeStream.write(sample + '\n');
        }

        writeStream.end();
        callback(null);
    });

    file.pipe(reader);
}

// Start the server
const port = 3000;
app.listen(port, () => {
    console.log(`Server running on http://localhost:${port}`);
});
