/*
 * ArduinoHuffman 
 * Authors: Rafael Perez
 */

#ifndef HUFFMAN_COMPRESSION_H
#define HUFFMAN_COMPRESSION_H

#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <bitset>

// Node class for the Huffman Tree
class Node
{
public:
    unsigned char character;
    int frequency;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    Node(unsigned char ch, int freq) : character(ch), frequency(freq), left(nullptr), right(nullptr) {}

    // Custom comparator for priority queue
    struct Compare
    {
        bool operator()(std::shared_ptr<Node> const &l, std::shared_ptr<Node> const &r)
        {
            return l->frequency > r->frequency;
        }
    };
};

// Function prototypes

// Function to build the frequency table from the input text
std::unordered_map<unsigned char, int> build_frequency_table(const std::string &text);

// Function to build the Huffman Tree from the frequency table
std::shared_ptr<Node> build_huffman_tree(const std::unordered_map<unsigned char, int> &frequency_table);

// Function to build the Huffman codes from the Huffman Tree
void build_huffman_codes(const std::shared_ptr<Node> &root, const std::string &code,
                         std::unordered_map<unsigned char, std::string> &huffman_codes);

// Function to compress a file using Huffman encoding
void compress_file(const std::string &input_filename, const std::string &output_filename);

// Helper function to write Huffman dictionary to the file
void write_huffman_dictionary(std::ofstream &outfile, const std::unordered_map<unsigned char, std::string> &huffman_codes);

// Helper function to encode the text using the Huffman codes
std::string encode_text(const std::string &text, const std::unordered_map<unsigned char, std::string> &huffman_codes);

// Function to decompress a file using Huffman encoding
void decompress_file(const std::string &huffman_filename, const std::string &output_filename);

// Helper function to read Huffman dictionary from the compressed file
void read_huffman_dictionary(std::ifstream &infile, std::shared_ptr<Node> &root);

// Helper function to decode the binary data using the Huffman tree
std::string decode_data(std::ifstream &infile, std::shared_ptr<Node> &root, int encoded_length);


#endif // HUFFMAN_COMPRESSION_H