import numpy as np
import matplotlib.pyplot as plt
import sys
import os

class HexDataVisualizer:
    def __init__(self, filepath):
        """
        Initialize the visualizer with data from a .hex file
        
        Args:
            filepath: Path to the .hex file containing int16 data
            Format: each line has 32-bit hex (8 chars) with two 16-bit values
            Example: 00FF0000 -> real=0x00FF (255), imag=0x0000 (0)
        """
        self.filepath = filepath
        self.real_data = []
        self.imag_data = []
        self.load_data()
        
    def load_data(self):
        """Load and parse the .hex file"""
        try:
            with open(self.filepath, 'r') as file:
                for line_num, line in enumerate(file, 1):
                    # Strip whitespace and skip empty lines
                    line = line.strip()
                    if not line:
                        continue
                    
                    try:
                        # Parse 32-bit hex value
                        value = int(line, 16)
                        
                        # Extract real (upper 16 bits) and imag (lower 16 bits)
                        real_val = (value >> 16) & 0xFFFF
                        imag_val = value & 0xFFFF
                        
                        # Convert to signed int16
                        if real_val >= 0x8000:
                            real_val = real_val - 0x10000
                        if imag_val >= 0x8000:
                            imag_val = imag_val - 0x10000
                        
                        self.real_data.append(real_val)
                        self.imag_data.append(imag_val)
                        
                    except ValueError as e:
                        print(f"Warning: Line {line_num} - '{line}' - {e}. Skipping.")
                        continue
                        
        except FileNotFoundError:
            print(f"Error: File '{self.filepath}' not found.")
            sys.exit(1)
        except Exception as e:
            print(f"Error reading file: {e}")
            sys.exit(1)
            
        # Convert to numpy arrays
        self.real_data = np.array(self.real_data)
        self.imag_data = np.array(self.imag_data)
        
        print(f"Loaded {len(self.real_data)} data points from {self.filepath}")
        if len(self.real_data) > 0:
            print(f"Real range: {np.min(self.real_data)} to {np.max(self.real_data)}")
            print(f"Imag range: {np.min(self.imag_data)} to {np.max(self.imag_data)}")
        
    def plot_raw(self):
        """Plot raw real and imaginary data"""
        if len(self.real_data) == 0:
            print("No data to plot")
            return
            
        fig, axes = plt.subplots(2, 1, figsize=(12, 8))
        
        # Real part
        axes[0].plot(self.real_data, 'b-', linewidth=1)
        axes[0].set_title('Real Part (I) - int16')
        axes[0].set_xlabel('Sample Index')
        axes[0].set_ylabel('Amplitude')
        axes[0].grid(True, alpha=0.3)
        
        # Imaginary part
        axes[1].plot(self.imag_data, 'r-', linewidth=1)
        axes[1].set_title('Imaginary Part (Q) - int16')
        axes[1].set_xlabel('Sample Index')
        axes[1].set_ylabel('Amplitude')
        axes[1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()

def main():
    """Main function to run the visualizer"""
    # If no file specified, use default or ask
    if len(sys.argv) > 1:
        filepath = sys.argv[1]
    else:
        # Create some sample data for demonstration if no file provided
        print("No file specified. Generating sample data for demonstration...")
        sample_file = "sample_data.hex"
        
        # Generate sample data (int16 values)
        num_points = 100
        t = np.linspace(0, 10, num_points)
        real = (np.sin(t) * 10000 + np.random.normal(0, 500, num_points)).astype(np.int16)
        imag = (np.cos(t) * 10000 + np.random.normal(0, 500, num_points)).astype(np.int16)
        
        # Write to hex file in format: 32-bit hex with real in upper 16 bits, imag in lower 16 bits
        with open(sample_file, 'w') as f:
            for r, i in zip(real, imag):
                # Convert to unsigned for packing
                r_unsigned = r & 0xFFFF
                i_unsigned = i & 0xFFFF
                # Pack into 32-bit value
                packed = (r_unsigned << 16) | i_unsigned
                f.write(f"{packed:08X}\n")
        
        filepath = sample_file
        print(f"Created sample file: {sample_file}")
    
    # Create visualizer and show plot
    visualizer = HexDataVisualizer(filepath)
    visualizer.plot_raw()

if __name__ == "__main__":
    main()