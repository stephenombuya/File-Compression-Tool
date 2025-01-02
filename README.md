# File Compression Tool

A simple command-line utility written in C for compressing and decompressing files using the zlib library.

## Features
- Compresses files using zlib's `deflate` method.
- Decompresses files using zlib's `inflate` method.
- Handles large files efficiently by processing data in chunks.
- Provides error handling for common issues like file access and invalid input.

## Requirements
- GCC compiler
- zlib library

### Installing zlib
On Debian/Ubuntu:
```bash
sudo apt-get install zlib1g-dev
```
On CentOS/Red Hat:
```bash
sudo yum install zlib-devel
```

## Compilation
Use the following command to compile the tool:
```bash
gcc -o file_compression_tool file_compression_tool.c -lz
```

## Usage
The tool supports two operations: compressing and decompressing files.

### Syntax
```bash
./file_compression_tool [compress|decompress] <input_file> <output_file>
```

### Examples
#### Compressing a File
```bash
./file_compression_tool compress input.txt compressed.zlib
```
This compresses `input.txt` into `compressed.zlib`.

#### Decompressing a File
```bash
./file_compression_tool decompress compressed.zlib output.txt
```
This decompresses `compressed.zlib` back into `output.txt`.

## How It Works
### Compression
- Reads the input file in chunks.
- Compresses each chunk using zlib's `deflate` function.
- Writes the compressed data to the output file.

### Decompression
- Reads the compressed file in chunks.
- Decompresses each chunk using zlib's `inflate` function.
- Writes the decompressed data to the output file.

## File Structure
```plaintext
main.c  # Main source code file
```

## Error Handling
- Handles errors such as:
  - Missing input/output files.
  - Issues during file read/write operations.
  - Invalid file format for decompression.

## Contribution
Contributions are welcome! Feel free to fork this repository and submit pull requests.

### Steps to Contribute
1. Fork the repository.
2. Create a new branch:
   ```bash
   git checkout -b feature-branch
   ```
3. Commit your changes:
   ```bash
   git commit -m "Add new feature"
   ```
4. Push the branch:
   ```bash
   git push origin feature-branch
   ```
5. Open a pull request.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgments
- The zlib library for providing robust compression and decompression functionalities.

## Contact
For any questions or feedback, feel free to reach out:
- **GitHub**: [stephenombuya](https://github.com/stephenombuya)

