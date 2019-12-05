#include <src/FastaParser.h>
#include <src/InputParser.h>
#include <src/ctre.hpp>

// #include <iostream>
#include <vector>

static constexpr std::string_view PATTERN_FOO = "([EI])([TI])";

enum ResultField
{
	BEGIN = 0,
	END = 1,
	MATCH = 2
};

using MatchingResultType = std::tuple<uint16_t, uint16_t, std::string_view>;

/* Processes a single regex at a time passed as a template parameter.
 *
 **/
template <auto& pattern>
std::vector<MatchingResultType> process_regex(const std::string& sequence)
{
	std::vector<MatchingResultType> result;
	for (const auto& match : ctre::range<pattern>(sequence))
	{
		auto begin = match.template get<0>().begin();
		auto end = std::prev(match.template get<0>().end());
		result.emplace_back(std::distance(sequence.begin(), begin),
			std::distance(sequence.begin(), end), match.template get<0>().to_view());
	}
	return result;
}

/* Checks all the regex patterns for a given FASTA entry.
 * 
 * param name_seq_pair: 
 *
 **/
void check_patterns(const std::pair<std::string, std::string>& name_seq_pair)
{
	std::vector<std::pair<uint16_t, uint16_t>> result_vector;
	auto foo_results = process_regex<PATTERN_FOO>(name_seq_pair.second);
	for (const auto& result : foo_results)
	{
		result_vector.emplace_back(std::get<BEGIN>(result), std::get<END>(result));
		// std::cout << "match: " << std::get<MATCH>(result) << "\n";
		// std::cout << "pos: " << std::get<BEGIN>(result) << "-" << std::get<END>(result) << "\n";
	}
}

int main(int argc, char** argv)
{
	InputParser input(argc, argv);
	const std::string& filename = input.getCmdOption("-f");

	if (filename.empty())
	{
		std::cout << "-f is required!\n";
		return 0;
	}

	auto parser = FastaParser(filename);
	for (const auto& entry : parser)
	{
		// std::cout << entry.first << ", " << entry.second << "\n";
		check_patterns(entry);
	}

	return 1;
}