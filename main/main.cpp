
#include <proxy/ProxyAPI.h>
#include <proxy/NullParserPlugin.h>
#include <dnp3cpp/DNP3Factory.h>

#include "CommandLineOptions.h"
#include "easylogging++.h"



INITIALIZE_EASYLOGGINGPP

using namespace proxy;

// Input ("dirty") buffer for the parser.
// This is so we can tag the buffer for the elfbac policy close to the program entry point.
// In the future we'll likely pass the parser an abstract allocator instead of the raw buffer itself.
__attribute__((section(".data.inputbuffer")))
uint8_t inputBuffer[4619];

std::unique_ptr<IParserFactory> GetFactory(const std::string& name);

int main (int argc, char *argv[])
{
    CommandLineOptions options;
    options.Parse(argc, argv);

    ProxyConfig config;
    std::error_code ec;
    if(!options.Get(config, ec))
    {
        LOG(ERROR) << "Config error: " << ec.message();
        return -1;
    }

    LOG(INFO) << "Using parser: " << options.parser.getValue();

    auto factory = GetFactory(options.parser.getValue());

    if(config.observeOnly)
    {
        LOG(WARNING) << "Proxy is in OBSERVE ONLY mode";
    }

    if(config.exitAfterSession)
    {
        LOG(WARNING) << "Proxy will EXIT after processing a single TCP session";
    }

	proxy::Run(config, *factory, ec);
	
	if(ec)
	{
        LOG(ERROR) << "Proxy error: " << ec.message();
	    return -1;
	}
	
	return 0;
}

std::unique_ptr<IParserFactory> GetFactory(const std::string& name)
{
    if(name == "null")
    {
        return std::unique_ptr<IParserFactory>(new NullParserPluginFactory(4096));
    }
    else if(name == "dnp3")
    {
        return std::unique_ptr<IParserFactory>(
                new dnp3::DNP3Factory(WSlice(inputBuffer, sizeof(inputBuffer)))
        );
    }
    else
    {
        LOG(ERROR) << "Unknown parser: " << name;
        throw std::invalid_argument(name);
    }
}
