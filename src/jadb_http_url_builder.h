#ifndef JADB_HTTP_URL_BUILDER
#define JADB_HTTP_URL_BUILDER

#include <vector>
#include <string>

namespace jadb
{
	class UrlBuilder
	{
	public:
		UrlBuilder() = default;
		~UrlBuilder() = default;

		UrlBuilder& string();
		UrlBuilder& word(std::string work);
		UrlBuilder& number();

		std::string url();

		std::vector<std::string> parse(std::string& url) const;
	private:
		std::vector<std::string> m_componenets;
	};
}
#endif // JADB_HTTP_URL_BUILDER