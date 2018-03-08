#include "Conf.hpp"

namespace Cortex
{
	std::deque<std::string> split_keys(const std::string& _key_seq)
	{
		std::deque<std::string> keys;
		std::stringstream ss(_key_seq);
		std::string key;

		while ( std::getline( ss, key, '.' ) )
		{
			keys.push_back( key );
		}
		return keys;
	}

	bool traverse(json& _j, json::iterator& _it, const std::string& _key_seq)
	{
		std::deque<std::string> keys(split_keys(_key_seq));
		std::string key(keys.front());
		keys.pop_front();

		while (true)
		{
			if ((_it = _j.find(key)) == _j.end())
			{
				break;
			}

			if (keys.empty())
			{
				/// Key found
				return true;
			}
			else
			{
				if (_it->is_object())
				{
					key = keys.front();
					keys.pop_front();
					_j = _it.value();
					_it = _j.begin();
				}
				else
				{
					break;
				}
			}
		}
		return false;
	}

	bool parse_json(const std::string& _file, json& _json_root)
	{
		// Open, parse and load the JSON file
		std::ifstream is(_file);
		if (!is.is_open())
		{
			dlog() << "### Invalid configuration file " << _file;
			return false;
		}
		std::stringstream ss_json;
		ss_json << is.rdbuf();
		std::string content(ss_json.str());

		if ( content.empty() )
		{
			dlog() << "### Empty configuration file.";
			return false;
		}

		_json_root = json::parse(content);

		return true;
	}

	void check_array(const json& _j)
	{
		if (!_j.is_array())
		{
			dlog() << "### Error loading set " << _j.dump(4)
				   << "\nJSON entity is not an array.";
			exit(EXIT_FAILURE);
		}
	}

	void check_object(const json& _j)
	{
		if (!_j.is_object())
		{
			dlog() << "### Error loading map " << _j.dump(4)
				   << "\nJSON entity is not an object.";
			exit(EXIT_FAILURE);
		}
	}
}
