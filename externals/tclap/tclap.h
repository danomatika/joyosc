/*==============================================================================
	Dan Wilcox <danomatika@gmail.com>, 2009, 2010
==============================================================================*/
#ifndef TCLAP_H
#define TCLAP_H

#include <tclap/CmdLine.h>

namespace TCLAP {

// custom output class to print usage the way I want it aka like goptions
class Output : public StdOutput
{
	public:

		/**
		 * Prints the usage to stdout.  Can be overridden to
		 * produce alternative behavior.
		 * \param c - The CmdLine object the output is generated for.
		 */
		virtual void usage(CmdLineInterface& c)
		{
			shortUsage(c, std::cout);
			std::cout << std::endl;
			
			spacePrint(std::cout, c.getProgramName()+": "+c.getMessage(), 80, 2,
					   c.getProgramName().length()+2);
			std::cout << std::endl;
			
			longUsage(c, std::cout);
			std::cout << std::endl;
		}

	protected:

		/**
		 * Writes a brief usage message with short args.
		 * \param c - The CmdLine object the output is generated for.
		 * \param os - The stream to write the message to.
		 */
		void shortUsage(CmdLineInterface& c, std::ostream& os) const;

		/**
		 * Writes a longer usage message with long and short args,
		 * provides descriptions and prints message.
		 * \param c - The CmdLine object the output is generated for.
		 * \param os - The stream to write the message to.
		 */
		void longUsage(CmdLineInterface& c, std::ostream& os) const;
		
		/**
		 * Makes a long id line for the argument
		 * \parm arg - The Argument to make the longID for.
		 * \return The formatted long id line.
		 */
		std::string makeLongID(Arg* arg) const;
		
		/**
		 * Print a line with a specified right margin for a second line of text
		 * \param os - The CmdLine object the output is generated for.
		 * \param first - The left margined line of text.
		 * \param second - The right margined line of text.
		 * \param indent - How much to indent the entire line.
		 * \param secondIndent - Where the right margin starts for the second
		 * 						 line of text.
		 */
		void printLine(std::ostream& os, std::string first, std::string second,
					   int indent, int secondIndent) const;
};

// TCLAP CmdLine wrapper to add custon Output class
class CommandLine : public CmdLine
{
	public:

		/**
		 * Command line constructor. Defines how the arguments will be
		 * parsed.
		 * \param message - The message to be used in the usage
		 * output.
		 * \param delimiter - The character that is used to separate
		 * the argument flag/name from the value.  Defaults to ' ' (space).
		 * \param version - The version number to be used in the
		 * --version switch.
		 * \param helpAndVersion - Whether or not to create the Help and
		 * Version switches. Defaults to true.
		 */
		CommandLine(const std::string& message,
					const std::string& version = "none")
			: CmdLine(message, ' ', version, true)
			{this->setOutput(&output);}

	private:

		// custom output class
		Output output;
};

inline void Output::shortUsage(CmdLineInterface& _cmd, std::ostream& os) const
{
	std::list<Arg*> argList = _cmd.getArgList();
	std::string progName = _cmd.getProgramName();
	XorHandler xorHandler = _cmd.getXorHandler();
	std::vector< std::vector<Arg*> > xorList = xorHandler.getXorList();

	std::string s = "USAGE: " + progName;
	int secondLineOffset = s.length() + 1;
 
	// first the xor
	for(int i = 0; static_cast<unsigned int>(i) < xorList.size(); i++)
	{
		s += " {";
		for(ArgVectorIterator it = xorList[i].begin();
			it != xorList[i].end(); it++)
		{
			s += (*it)->shortID() + "|";
		}
		s[s.length()-1] = '}';
	}

	// then the rest
	for(ArgListIterator it = argList.begin(); it != argList.end(); it++)
	{
		if(!xorHandler.contains((*it)) )// && (*it)->getName() != "")
		{
			s += " " + (*it)->shortID();
		}
	}
	
	spacePrint(os, s, 80, 0, secondLineOffset);
}

inline void Output::longUsage(CmdLineInterface& _cmd, std::ostream& os) const
{
	std::list<Arg*> argList = _cmd.getArgList();
	std::string message = _cmd.getMessage();
	XorHandler xorHandler = _cmd.getXorHandler();
	std::vector< std::vector<Arg*> > xorList = xorHandler.getXorList();

	unsigned int longestIdLen = 0;
	std::string msg = "";
	bool bPrintCommands = false;

	// find the longest id length to use it as the right margin
	for(ArgListIterator it = argList.begin(); it != argList.end(); it++)
	{
		msg = makeLongID((*it));
			if(msg.length() > longestIdLen)
		{
			longestIdLen = msg.length();
		}
		
		// is this a command?
		if(!xorHandler.contains((*it)) && (*it)->getFlag() == "" &&
		  ((*it)->getName() != "help" && (*it)->getName() != "version"))
		{
			bPrintCommands = true;
			
			// commands are printed differently
			if((*it)->shortID().length() > longestIdLen)
			{
				longestIdLen = (*it)->shortID().length();
			}
		}
	}

	// begin options
	spacePrint(os, "Options:", 80, 0, 0);

	// xor args first
	for(int i = 0; static_cast<unsigned int>(i) < xorList.size(); i++)
	{
		for(ArgVectorIterator it = xorList[i].begin();
			it != xorList[i].end(); ++it)
		{
			printLine(os, makeLongID((*it)),
					  (*it)->getDescription(), 2, longestIdLen);
			if(it+1 != xorList[i].end())
			{
				printLine(os, "", "-- OR --", 3, longestIdLen);
			}
			
			// remove arg from cmd list
			argList.remove((*it));
		}
		os << std::endl;
	}
	
	// rest of the args
	for(ArgListIterator it = argList.begin(); it != argList.end();)
	{
		if((*it)->shortID().at(0) != '<') // ignore commands which begin with '<'
		{
			printLine(os, makeLongID((*it)),
					  (*it)->getDescription(), 2, longestIdLen);
					  
			// remove arg from cmd list
			it = argList.erase(it);
		}
		else
		{
			++it;
		}
	}
	os << std::endl;
	
	// commands are the only thing left now
	if(bPrintCommands)
	{
		spacePrint(os, "Commands: (in order)", 80, 0, 0);
		for(ArgListIterator it = argList.begin(); it != argList.end(); it++)
		{ 
			printLine(os, (*it)->shortID(),
					(*it)->getDescription(), 2, longestIdLen);
		}
	}
}

inline std::string Output::makeLongID(Arg* arg) const
{
	std::string id = "";
	
	if(arg->getFlag() != "")
	{
		id += "-" + arg->getFlag();
		
		if(arg->getName() != "")
		{
			id += ", ";
		}
	}
	
	if (arg->getName() != "")
	{
		id += "--" + arg->getName();
	}
	
	return id;
}

inline void Output::printLine(std::ostream& os, std::string first,
												std::string second,
												int indent,
												int secondIndent) const
{
	std::string line = first + "    ";
	int padLimit = secondIndent - line.length() + 4 + indent;
	for(int i = 0; i < padLimit; ++i)
		line += ' ';
		
	line += second;
	spacePrint(os, line, 80, indent, secondIndent + 4 + indent);
}

} // namespace

#endif // TCLAP_H
