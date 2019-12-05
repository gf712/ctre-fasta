#include <iterator>
#include <type_traits>
// #include <iostream>

template <typename FileInputType>
class FileIterator
{
public:
	template <typename InternalFileInputType>
	class iterator
	{
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = typename std::conditional<
			std::is_const_v<typename std::remove_pointer_t<InternalFileInputType>>,
			const typename InternalFileInputType::ReturnType,
			typename InternalFileInputType::ReturnType>::type;
		using difference_type = ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		iterator(InternalFileInputType* parser)
			: m_parser(parser)
			, m_cached_line(m_parser->get_current_entry())
			, m_current_position(m_parser->get_position())
		{
		}

		iterator(InternalFileInputType* parser, int idx)
			: m_parser(parser)
			, m_cached_line(m_parser->get_current_entry())
			, m_current_position(idx)
		{
		}

		iterator& operator++()
		{
			// std::cout << "iterator& operator++()\n";
			// std::cout << m_cached_line.first << ", " << m_cached_line.second <<
			// "\n";
			m_current_position = m_parser->get_position();
			// std::cout << m_current_position << "\n";
			m_cached_line = m_parser->readline();
			return *this;
		}

		iterator operator++(int)
		{
			iterator retval(*this);
			++(*this);
			return retval;
		}

		bool operator==(const iterator& other) const
		{
			return (this->m_parser->get_filename() == other.m_parser->get_filename())
				&& (this->m_current_position == other.m_current_position);
		}

		bool operator!=(const iterator& other) const { return !(*this == other); }

		reference operator*() const
		{
			// std::cout << "reference operator*()\n";
			// std::cout << m_parser->get_current_entry().first << ", " <<
			// m_parser->get_current_entry().second << "\n";
			return m_cached_line;
		}

	private:
		InternalFileInputType* m_parser;
		reference m_cached_line;
		int m_current_position;
	};

	iterator<FileInputType> begin()
	{
		auto* this_casted = static_cast<FileInputType*>(this);
		return iterator<FileInputType>(this_casted, 0);
	}

	iterator<FileInputType> end()
	{
		auto* this_casted = static_cast<FileInputType*>(this);
		return iterator<FileInputType>(this_casted, -1);
	}

	// const doesn't make sense with the parser design as it is stateful
	// iterator<const FileInputType> begin() const
	// {
	// 	const auto* this_casted = static_cast<const FileInputType*>(this);
	// 	return iterator<const FileInputType>(this_casted, 0);
	// }

	// iterator<const FileInputType> end() const
	// {
	// 	const auto* this_casted = static_cast<const FileInputType*>(this);
	// 	return iterator<const FileInputType>(this_casted, -1);
	// }
};
