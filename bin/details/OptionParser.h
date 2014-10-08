// OptionsParser.h
// Copyright (c) 2010-2012 Jes Frellsen
//
// This file is part of Muninn.
//
// Muninn is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.
//
// Muninn is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Muninn.  If not, see <http://www.gnu.org/licenses/>.
//
// The following additional terms apply to the Muninn software:
// Neither the names of its contributors nor the names of the
// organizations they are, or have been, associated with may be used
// to endorse or promote products derived from this software without
// specific prior written permission.

#ifndef OPTIONPARSER_H_
#define OPTIONPARSER_H_

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <cstdlib>

class OptionParser {
private:
    std::string program_name;
    std::string help_text;

    std::string additional_arguments_help_text;           // Helptext for additional arugments

    std::map<std::string, std::string> options;           // option -> destination map
    std::map<std::string, std::string> help;              // option -> help map
    std::set<std::string> destinations;                   // set of destinations found in options
    std::map<std::string, std::string> defaults;          // option -> default values map
    std::map<std::string, std::string> implicits;         // option -> implicit values map

    std::set<std::string> required;                       // The required options
    std::vector<std::string> order;                       // The order of options for help

    std::map<std::string, std::string> values;            // destination -> value map
    std::vector<std::string> additional_arguments;        // list of additional arguments

public:
    // Constructor
    OptionParser(const std::string &help_text="", const std::string &additional_arguments_help_text="") : help_text(help_text), additional_arguments_help_text(additional_arguments_help_text) {
        // Add the help option
        add_option("-h", "help", "Show this help message and exit", "0", "1");
    }

    // Parse the command line
    void parse_args(int argc, char *argv[]) {
        std::set<std::string> encountered;
        program_name = argv[0];

        // Set the default values
        for (std::map<std::string,std::string>::iterator it=defaults.begin(); it!=defaults.end(); ++it) {
            std::string dest = options[it->first];
            values[dest] = it->second;
        }

        // Parse the arguments
        for (int i=1; i<argc; ++i) {
            std::string arg = argv[i];

            // Parse the next argument
            if (arg.substr(0,1) == "-") {
                // Check if the option in valid
                if (options.count(arg) > 0) {
                    std::string dest = options[arg];

                    // Get the value
                    std::string value;

                    if (implicits.count(arg)== 0) {
                        if (i+1 == argc)
                            parser_error("Missing argument for option " + arg);
                        ++i;
                        value = argv[i];
                    }
                    else {
                        value = implicits[arg];
                    }

                    // Check that the option is unique and save the value
                    if (encountered.count(arg)>0)
                        parser_error("Duplicate option " + arg);
                    encountered.insert(arg);

                    // Save the option
                    values[dest] = value;
                }
                else {
                    parser_error("Unknown option " + arg);
                }
            } else {
                additional_arguments.push_back(arg);
            }
        }

        // Check if the help should be printed
        if (get_as<bool>("help")) {
            print_help();
            std::exit(0);
        }

        // Check if all required options are given
        for (std::set<std::string>::const_iterator it=required.begin(); it!=required.end(); ++it) {
            if (values.count(options[*it])==0)
                parser_error("Missing required option " + *it);
        }

    }

    // Adders for options
    enum ConstrainEnum {OPTINAL=0, REQUIRED};

    void add_option(const std::string &option, const std::string &dest, const std::string &help_text, ConstrainEnum constrain=OPTINAL) {
        // Check the validity of the option
        if (option.length() != 2)
            parser_setup_error("An options must have length 2");
        if (option.substr(0, 1) != "-")
            parser_setup_error("An options must start with -");
        if (!isalnum(option.substr(1, 2).c_str()[0]))
            parser_setup_error("An options must have the form -?, where ? is alpha numeric");

        // Check that it the first time the options is given
        if (options.count(option) > 0)
            parser_setup_error("Duplicate options: " + option);
        if (destinations.count(dest) > 0)
            parser_setup_error("Duplicate destination: " + dest);

        // Add the options
        options[option] = dest;
        help[option] = help_text;
        destinations.insert(dest);
        order.push_back(option);

        if (constrain==REQUIRED)
            this->required.insert(option);

    }

    void add_option(const std::string &option, const std::string &dest, const std::string &help_text, const std::string &default_value) {
        add_option(option, dest, help_text);
        defaults[option] = default_value;
    }

    void add_option(const std::string &option, const std::string &dest, const std::string &help_text, const std::string &default_value, const std::string &implicit_value) {
        add_option(option, dest, help_text, default_value);
        implicits[option] = implicit_value;
    }

    // Getters for options
    std::string get(const std::string &dest) {
        // Check that the destination is valid
        if (values.count(dest)==0)
            parser_setup_error("Unknown destination: " + dest);
        return values[dest];
    }

    template <typename T>
    T get_as(const std::string &dest) {
        // Check that the destination is valid
        if (values.count(dest)==0)
            parser_setup_error("Unknown destination: " + dest);

        // Transform the options to the given type
        std::istringstream iss(values[dest]);
        T value;
        iss >> value;
        return value;
    }

    bool has(const std::string &dest) {
        return values.count(dest);
    }

    // Get the list of additional arguments
    const std::vector<std::string>& get_additional_arguments() {
        return additional_arguments;
    }

    // Help printer
    void print_help() {
        std::string txt = "Usage: " + program_name + " [options]";

        if (additional_arguments_help_text != "") {
            txt += " [...]\n";
        }
        else {
            txt += "\n";
        }

        if (help_text!="") {
            txt += "\n" + help_text + "\n";
        }

        txt += "\nOptions:\n";

        for (std::vector<std::string>::const_iterator it=order.begin(); it!=order.end(); ++it) {
            txt += "  " + *it;

            if (implicits.count(*it)==0)
                txt += "  ARG";
            else
                txt += "     ";

            txt += "  " + help[*it];

            if (defaults.count(*it)>0 || implicits.count(*it)>0 || required.count(*it)) {
                txt += " [";

                if (required.count(*it)>0)
                    txt += "required]";

                if (implicits.count(*it)>0)
                    txt += "implicit=" + implicits[*it] + ", ";

                if (defaults.count(*it)>0)
                    txt += "default=" + defaults[*it] + "]";
            }
            txt += "\n";
        }

        if (additional_arguments_help_text != "") {
            txt += "     [...] " + additional_arguments_help_text + "\n";
        }

        std::cout << txt;
    }

    // Write error message
    void parser_error(const std::string &error_message) {
        std::cout << program_name << ": error: " << error_message << std::endl;
        std::exit(1);
    }

     void parser_setup_error(const std::string &error_message) {
        std::cout << "error setting up parser: " << error_message << std::endl;
        std::exit(1);
    }

     // Output
     friend std::ostream & operator<<(std::ostream &o, const OptionParser &option_parser) {
         for (std::map<std::string,std::string>::const_iterator it=option_parser.values.begin(); it!=option_parser.values.end(); ++it)
             o << it->first << " = " << it->second << std::endl;
         return o;
     }

};

#endif /* OPTIONPARSER_H_ */
