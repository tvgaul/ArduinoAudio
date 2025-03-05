import wave

def read_wav_file(file_path):
    try:
        with wave.open(file_path, "rb") as wav_file:
            print(f"Channels: {wav_file.getnchannels()}")
            print(f"Sample Width: {wav_file.getsampwidth()} bytes")
            print(f"Frame Rate (Sample Rate): {wav_file.getframerate()} Hz")
            print(f"Total Frames: {wav_file.getnframes()}")
            print(f"Duration: {wav_file.getnframes() / wav_file.getframerate()} seconds")
            
            frames = wav_file.readframes(wav_file.getnframes())  # Read all audio frames
            print(f"First 10 bytes of audio data: {frames[:10]}")  # Print first 10 bytes
    except FileNotFoundError:
        print("File not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

# Example usage

file_path = "C:\\Users\\King Chicken\\Documents\Audacity\\Virus_Alert.wav"  # Replace with your file path
read_wav_file(file_path)