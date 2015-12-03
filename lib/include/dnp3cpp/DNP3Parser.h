
#ifndef PROXYDNP3_DNP3PARSER_H
#define PROXYDNP3_DNP3PARSER_H

#include <plugin/ParserPlugin.h>
#include <plugin/Uncopyable.h>

#include <dnp3hammer.h>

namespace proxy { namespace  dnp3 {

    class DNP3Parser final : public IParser, private Uncopyable {

            friend class DNP3Factory;

            DNP3Parser(IParserCallbacks& callbacks);

        public:

            ~DNP3Parser();

            virtual WSlice GetWriteSlice() override;

            virtual bool Parse(size_t num) override;

        private:

            static DNP3_Callbacks GetCallbacks();

            static void OnLinkFrame(void *env, const DNP3_Frame *frame, const uint8_t *buf, size_t len);
            static void OnTransportSegment(void *env, const DNP3_Segment *segment);
            static void OnTransportPayload(void *env, const uint8_t *s, size_t n);
            static void OnAppInvalid(void *env, DNP3_ParseError e);
            static void OnAppFragment(void *env, const DNP3_Fragment *fragment, const uint8_t *buf, size_t len);
            static void OnLogError(void *env, const char *fmt, ...);

            IParserCallbacks& m_callbacks;
            StreamProcessor* m_plugin;
    };

}}


#endif //PROXY_DNP3PARSER_H