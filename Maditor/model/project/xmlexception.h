#pragma once


namespace Maditor {
	namespace Model {

		class XmlException : public std::logic_error {
			using std::logic_error::logic_error;
		};

	}
}