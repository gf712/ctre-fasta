#include "FileIterator.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

using FastaFieldType = std::pair<std::string, std::string>;

class FastaParser : public FileIterator<FastaParser>
{
	friend FileIterator<FastaParser>;

public:
	using ReturnType = FastaFieldType;

	FastaParser(const std::string& filename)
		: m_filename(filename)
		, m_file_done(false)
	{
		m_input_stream = std::ifstream(filename);
		if (m_input_stream.fail())
		{
			throw std::runtime_error("Error opening file\n");
		}
		readline();
	}

	FastaFieldType& get_current_entry() { return m_current; }

	const FastaFieldType& get_current_entry() const { return m_current; }

	FastaFieldType& readline()
	{
		// std::cout << "FastaFieldType& readline()\n";
		FastaFieldType& result = consume_entry();
		// std::cout << result.first << ", " << result.second << "\n";
		return result;
	}

	const std::string& get_filename() const { return m_filename; }

	int get_position() const { return m_position; }

protected:
	void set_to_begin()
	{
		m_input_stream.seekg(0, std::ios::beg);
		m_position = m_input_stream.tellg();
		m_file_done = false;
	}

	void set_to_end()
	{
		m_input_stream.seekg(0, std::ios::end);
		// go one more forward after end -> EOF
		m_input_stream.peek();
		m_position = m_input_stream.tellg();
		m_file_done = true;
	}

private:
	FastaFieldType& consume_entry()
	{
		// std::cout << "FastaFieldType& consume_entry()\n";
		// std::cout << "position: " << m_position << "\n";
		// std::cout << "position: " << m_position << "\n";

		// if this is the end of file return the last line that was parsed
		if (m_input_stream.eof())
		{
			m_file_done = true;
			return m_current;
		}

		// get sequence name
		while (std::getline(m_input_stream, m_current_line))
		{
			if (*m_current_line.begin() == '>')
			{
				// get sequence and ignore starting '>''
				m_current.first = m_current_line.substr(1);
				break;
			}
		}

		// get the sequence, which is the whole line following the name:
		// >NAME
		// SEQUENCE
		std::getline(m_input_stream, m_current_line);
		m_current.second = m_current_line;

		m_position = m_input_stream.tellg();

		return m_current;
	}

	std::string m_current_line;
	int m_position;
	FastaFieldType m_current;
	std::ifstream m_input_stream;
	std::string m_filename;
	bool m_file_done;
};
