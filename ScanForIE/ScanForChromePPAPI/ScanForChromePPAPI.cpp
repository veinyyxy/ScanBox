// ScanForChromePPAPI.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"


namespace {

	// The expected string sent by the browser.
	const char* const kHelloString = "hello";
	// The string sent back to the browser upon receipt of a message
	// containing "hello".
	const char* const kReplyString = "hello from NaCl";

}  // namespace

class HelloTutorialInstance : public pp::Instance {
public:
	explicit HelloTutorialInstance(PP_Instance instance)
		: pp::Instance(instance) {}
	virtual ~HelloTutorialInstance() {}

	virtual void HandleMessage(const pp::Var& var_message) {
		// Ignore the message if it is not a string.
		if (!var_message.is_string())
			return;

		// Get the string message and compare it to "hello".
		std::string message = var_message.AsString();
		if (message == kHelloString) {
			// If it matches, send our response back to JavaScript.
			pp::Var var_reply(kReplyString);
			PostMessage(var_reply);
		}
	}
};

class HelloTutorialModule : public pp::Module {
public:
	HelloTutorialModule() : pp::Module() {}
	virtual ~HelloTutorialModule() {}

	virtual pp::Instance* CreateInstance(PP_Instance instance) {
		return new HelloTutorialInstance(instance);
	}
};

namespace pp {

	Module* CreateModule() {
		return new HelloTutorialModule();
	}

}  // namespace pp
