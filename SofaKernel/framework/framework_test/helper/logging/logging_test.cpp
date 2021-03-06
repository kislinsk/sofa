#include <gtest/gtest.h>
#include <exception>
#include <algorithm>

#include <iostream>
using std::endl ;

#include <vector>
using std::vector ;

#include <sofa/helper/logging/Messaging.h>
using sofa::helper::logging::MessageDispatcher ;

#include <sofa/helper/logging/MessageHandler.h>
using sofa::helper::logging::MessageHandler ;

#include <sofa/helper/logging/Message.h>
using sofa::helper::logging::Message ;

#include <sofa/helper/logging/LoggingMessageHandler.h>
using sofa::helper::logging::MainLoggingMessageHandler ;
using sofa::helper::logging::LogMessage ;

#include <sofa/helper/logging/CountingMessageHandler.h>
using sofa::helper::logging::MainCountingMessageHandler ;
using sofa::helper::logging::CountingMessageHandler ;

#include <sofa/helper/logging/RoutingMessageHandler.h>
using sofa::helper::logging::RoutingMessageHandler ;
using sofa::helper::logging::MainRoutingMessageHandler ;

#include <sofa/helper/logging/ConsoleMessageHandler.h>
using sofa::helper::logging::ConsoleMessageHandler ;

#include <sofa/helper/logging/RichConsoleStyleMessageFormatter.h>
using sofa::helper::logging::RichConsoleStyleMessageFormatter ;

#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/ObjectFactory.h>

//TODO(dmarchal): replace that with the LoggingMessageHandler
class MyMessageHandler : public MessageHandler
{
    vector<Message> m_messages ;
public:
    virtual void process(Message& m){
        m_messages.push_back(m);

        //        if( !m.sender().empty() )
        //            std::cerr<<m<<std::endl;
    }

    size_t numMessages(){
        return m_messages.size() ;
    }

    const vector<Message>& messages() const {
        return m_messages;
    }
    const Message& lastMessage() const {
        return m_messages.back();
    }
} ;



TEST(LoggingTest, noHandler)
{
    MessageDispatcher::clearHandlers() ;

    msg_info("") << " info message with conversion" << 1.5 << "\n" ;
    msg_deprecated("") << " deprecated message with conversion" << 1.5 << "\n" ;
    msg_warning("") << " warning message with conversion "<< 1.5 << "\n" ;
    msg_error("") << " error message with conversion" << 1.5 << "\n" ;
}


TEST(LoggingTest, oneHandler)
{
    MessageDispatcher::clearHandlers() ;

    MyMessageHandler h;

    // add is expected to return the handler ID. Here is it the o'th
    EXPECT_TRUE(MessageDispatcher::addHandler(&h) == 0 ) ;

    msg_info("") << " info message with conversion" << 1.5 << "\n" ;
    msg_deprecated("") << " deprecated message with conversion" << 1.5 << "\n" ;
    msg_warning("") << " warning message with conversion "<< 1.5 << "\n" ;
    msg_error("") << " error message with conversion" << 1.5 << "\n" ;

    EXPECT_TRUE( h.numMessages() == 4u ) ;
}

TEST(LoggingTest, duplicatedHandler)
{
    MessageDispatcher::clearHandlers() ;

    MyMessageHandler h;

    // First add is expected to return the handler ID.
    EXPECT_TRUE(MessageDispatcher::addHandler(&h) == 0) ;

    // Second is supposed to fail to add and thus return -1.
    EXPECT_TRUE(MessageDispatcher::addHandler(&h) == -1) ;

    msg_info("") << " info message with conversion" << 1.5 << "\n" ;
    msg_deprecated("") << " deprecated message with conversion" << 1.5 << "\n" ;
    msg_warning("") << " warning message with conversion "<< 1.5 << "\n" ;
    msg_error("") << " error message with conversion" << 1.5 << "\n" ;

    EXPECT_TRUE( h.numMessages() == 4u) ;
}


TEST(LoggingTest, withoutDevMode)
{
    MessageDispatcher::clearHandlers() ;

    MyMessageHandler h;
    MessageDispatcher::addHandler(&h) ;

    msg_info("") << " info message with conversion" << 1.5 << "\n" ;
    msg_deprecated("") << " deprecated message with conversion" << 1.5 << "\n" ;
    msg_warning("") << " warning message with conversion "<< 1.5 << "\n" ;
    msg_error("") << " error message with conversion" << 1.5 << "\n" ;

    nmsg_info("") << " null info message with conversion" << 1.5 << "\n" ;
    nmsg_deprecated("") << " null deprecated message with conversion" << 1.5 << "\n" ;
    nmsg_warning("") << " null warning message with conversion "<< 1.5 << "\n" ;
    nmsg_error("") << " null error message with conversion" << 1.5 << "\n" ;

    EXPECT_TRUE( h.numMessages() == 4u ) ;
}

//TEST(LoggingTest, speedTest)
//{
//    MessageDispatcher::clearHandlers() ;

//    MyMessageHandler h;
//    MessageDispatcher::addHandler(&h) ;

//    for(unsigned int i=0;i<10000;i++){
//        msg_info("") << " info message with conversion" << 1.5 << "\n" ;
//        msg_warning("") << " warning message with conversion "<< 1.5 << "\n" ;
//        msg_error("") << " error message with conversion" << 1.5 << "\n" ;
//    }
//}



TEST(LoggingTest, emptyMessage)
{
    MessageDispatcher::clearHandlers() ;
    MyMessageHandler h;
    MessageDispatcher::addHandler(&h) ;

    // an empty message should not be processed

    msg_info("");
    EXPECT_EQ( h.numMessages(), 0u );

    msg_info("")<<"ok";
    msg_info("");
    EXPECT_EQ( h.numMessages(), 1u );
}





class MyComponent : public sofa::core::objectmodel::BaseObject
{
public:
    MyComponent()
    {
        f_printLog.setValue(true); // to print sout
        serr<<"regular serr"<<sendl;
        sout<<"regular sout"<<sendl;
        serr<<SOFA_FILE_INFO<<"serr with fileinfo"<<sendl;
        sout<<SOFA_FILE_INFO<<"sout with fileinfo"<<sendl;
    }
};

TEST(LoggingTest, BaseObject)
{
    MessageDispatcher::clearHandlers() ;
    MyMessageHandler h;
    MessageDispatcher::addHandler(&h) ;


    MyComponent c;

    /// the constructor of MyComponent is sending 4 messages
    EXPECT_EQ( h.numMessages(), 4u ) ;

    c.serr<<"regular external serr"<<c.sendl;
    EXPECT_EQ( h.lastMessage().fileInfo()->line, 0 );
    EXPECT_TRUE( !strcmp( h.lastMessage().fileInfo()->filename, sofa::helper::logging::s_unknownFile ) );
    EXPECT_EQ( h.lastMessage().type(), sofa::helper::logging::Message::Warning );
    EXPECT_EQ( h.lastMessage().context(), sofa::helper::logging::Message::Runtime );

    c.serr<<sofa::helper::logging::Message::Error<<"external serr as Error"<<c.sendl;
    EXPECT_EQ( h.lastMessage().fileInfo()->line, 0 );
    EXPECT_TRUE( !strcmp( h.lastMessage().fileInfo()->filename, sofa::helper::logging::s_unknownFile ) );
    EXPECT_EQ( h.lastMessage().type(), sofa::helper::logging::Message::Error );
    EXPECT_EQ( h.lastMessage().context(), sofa::helper::logging::Message::Runtime );

    c.sout<<"regular external sout"<<c.sendl;
    EXPECT_EQ( h.lastMessage().fileInfo()->line, 0 );
    EXPECT_TRUE( !strcmp( h.lastMessage().fileInfo()->filename, sofa::helper::logging::s_unknownFile ) );
    EXPECT_EQ( h.lastMessage().type(), sofa::helper::logging::Message::Info );
    EXPECT_EQ( h.lastMessage().context(), sofa::helper::logging::Message::Runtime );

    c.sout<<sofa::helper::logging::Message::Error<<"external sout as Error"<<c.sendl;
    EXPECT_EQ( h.lastMessage().fileInfo()->line, 0 );
    EXPECT_TRUE( !strcmp( h.lastMessage().fileInfo()->filename, sofa::helper::logging::s_unknownFile ) );
    EXPECT_EQ( h.lastMessage().type(), sofa::helper::logging::Message::Error );
    EXPECT_EQ( h.lastMessage().context(), sofa::helper::logging::Message::Runtime );


    c.serr<<SOFA_FILE_INFO<<"external serr with fileinfo"<<c.sendl;
    EXPECT_EQ( h.lastMessage().fileInfo()->line, __LINE__-1 );
    EXPECT_TRUE( !strcmp( h.lastMessage().fileInfo()->filename, __FILE__ ) );

    c.sout<<SOFA_FILE_INFO<<"external sout with fileinfo"<<c.sendl;
    EXPECT_EQ( h.lastMessage().fileInfo()->line, __LINE__-1 );
    EXPECT_TRUE( !strcmp( h.lastMessage().fileInfo()->filename, __FILE__ ) );

    c.serr<<SOFA_FILE_INFO<<sofa::helper::logging::Message::Error<<"external serr as Error with fileinfo"<<c.sendl;
    EXPECT_EQ( h.lastMessage().fileInfo()->line, __LINE__-1 );
    EXPECT_TRUE( !strcmp( h.lastMessage().fileInfo()->filename, __FILE__ ) );
    EXPECT_EQ( h.lastMessage().type(), sofa::helper::logging::Message::Error );

    c.sout<<sofa::helper::logging::Message::Error<<SOFA_FILE_INFO<<"external sout as Error with fileinfo"<<c.sendl;
    EXPECT_EQ( h.lastMessage().fileInfo()->line, __LINE__-1 );
    EXPECT_TRUE( !strcmp( h.lastMessage().fileInfo()->filename, __FILE__ ) );
    EXPECT_EQ( h.lastMessage().type(), sofa::helper::logging::Message::Error );
    EXPECT_EQ( h.lastMessage().context(), sofa::helper::logging::Message::Runtime );

    c.serr<<"serr with sendl that comes in a second time";
    c.serr<<c.sendl;
    EXPECT_EQ( h.lastMessage().fileInfo()->line, 0 );
    EXPECT_TRUE( !strcmp( h.lastMessage().fileInfo()->filename, sofa::helper::logging::s_unknownFile ) );
    EXPECT_EQ( h.lastMessage().type(), sofa::helper::logging::Message::Warning );

    c.serr<<"\n serr with \n end of "<<std::endl<<" lines"<<c.sendl;
    EXPECT_EQ( h.lastMessage().fileInfo()->line, 0 );
    EXPECT_TRUE( !strcmp( h.lastMessage().fileInfo()->filename, sofa::helper::logging::s_unknownFile ) );
    EXPECT_EQ( h.lastMessage().type(), sofa::helper::logging::Message::Warning );

    c.serr<<sofa::helper::logging::Message::Dev<<sofa::helper::logging::Message::Error<<"external Dev serr"<<c.sendl;
    EXPECT_EQ( h.lastMessage().type(), sofa::helper::logging::Message::Error );
    EXPECT_EQ( h.lastMessage().context(), sofa::helper::logging::Message::Dev );


    EXPECT_EQ( h.numMessages(), 15u ) ;

    // an empty message should not be processed
    c.serr<<c.sendl;

    EXPECT_EQ( h.numMessages(), 15u ) ;

}

#undef MESSAGING_H
#define WITH_SOFA_DEVTOOLS
#undef dmsg_info
#undef dmsg_deprecated
#undef dmsg_error
#undef dmsg_warning
#undef dmsg_fatal
#undef dmsg_advice
#include <sofa/helper/logging/Messaging.h>

TEST(LoggingTest, withDevMode)
{
    MessageDispatcher::clearHandlers() ;

    MyMessageHandler h;
    MessageDispatcher::addHandler(&h) ;

    msg_info("") << " info message with conversion" << 1.5 << "\n" ;
    msg_warning("") << " warning message with conversion "<< 1.5 << "\n" ;
    msg_error("") << " error message with conversion" << 1.5 << "\n" ;

    nmsg_info("") << " null info message with conversion" << 1.5 << "\n" ;
    nmsg_warning("") << " null warning message with conversion "<< 1.5 << "\n" ;
    nmsg_error("") << " null error message with conversion" << 1.5 << "\n" ;

    dmsg_info("") << " debug info message with conversion" << 1.5 << "\n" ;
    dmsg_warning("") << " debug warning message with conversion "<< 1.5 << "\n" ;
    dmsg_error("") << " debug error message with conversion" << 1.5 << "\n" ;

    EXPECT_TRUE( h.numMessages() == 6u ) ;
}



TEST(LoggingTest, checkLoggingMessageHandler)
{
    CountingMessageHandler& m = MainCountingMessageHandler::getInstance() ;
    m.reset();

    MessageDispatcher::clearHandlers() ;
    MessageDispatcher::addHandler( &MainLoggingMessageHandler::getInstance() );
    LogMessage loggingFrame;

    msg_info("") << " info message with conversion" << 1.5 << "\n" ;
    msg_warning("") << " warning message with conversion "<< 1.5 << "\n" ;
    msg_error("") << " error message with conversion" << 1.5 << "\n" ;

    nmsg_info("") << " null info message with conversion" << 1.5 << "\n" ;
    nmsg_warning("") << " null warning message with conversion "<< 1.5 << "\n" ;
    nmsg_error("") << " null error message with conversion" << 1.5 << "\n" ;

    if( loggingFrame.size() != 3 )
    {
        EXPECT_EQ( loggingFrame.size(), 3) ;
        for(auto& message : loggingFrame)
        {
            std::cout << message << std::endl ;
        }
    }

}

TEST(LoggingTest, checkCountingMessageHandler)
{
    CountingMessageHandler& m = MainCountingMessageHandler::getInstance() ;
    m.reset();

    MessageDispatcher::clearHandlers() ;
    MessageDispatcher::addHandler( &m );

    std::vector<Message::Type> errortypes = {Message::Error, Message::Warning, Message::Info,
                                             Message::Advice, Message::Deprecated, Message::Fatal} ;

    for(auto& type : errortypes)
    {
        EXPECT_EQ(m.getMessageCountFor(type), 0) ;
    }

    int i = 0 ;
    for(auto& type : errortypes)
    {
        i++;
        msg_info("") << " info message with conversion" << 1.5 << "\n" ;
        msg_warning("") << " warning message with conversion "<< 1.5 << "\n" ;
        msg_error("") << " error message with conversion" << 1.5 << "\n" ;
        msg_fatal("") << " fatal message with conversion" << 1.5 << "\n" ;
        msg_deprecated("") << " deprecated message with conversion "<< 1.5 << "\n" ;
        msg_advice("") << " advice message with conversion" << 1.5 << "\n" ;

        nmsg_info("") << " null info message with conversion" << 1.5 << "\n" ;
        nmsg_warning("") << " null warning message with conversion "<< 1.5 << "\n" ;
        nmsg_error("") << " null error message with conversion" << 1.5 << "\n" ;
        nmsg_fatal("") << " fatal message with conversion" << 1.5 << "\n" ;
        nmsg_deprecated("") << " deprecated message with conversion "<< 1.5 << "\n" ;
        nmsg_advice("") << " advice message with conversion" << 1.5 << "\n" ;

        EXPECT_EQ(m.getMessageCountFor(type), i) ;
    }
}

TEST(LoggingTest, checkRoutingMessageHandler)
{
    RoutingMessageHandler& m = MainRoutingMessageHandler::getInstance() ;

    MessageDispatcher::clearHandlers() ;
    MessageDispatcher::addHandler( &m );

    std::vector<Message::Type> errortypes = {Message::Error, Message::Warning, Message::Info,
                                             Message::Advice, Message::Deprecated, Message::Fatal} ;

    RichConsoleStyleMessageFormatter* fmt = new RichConsoleStyleMessageFormatter();
    ConsoleMessageHandler* consolehandler = new ConsoleMessageHandler(fmt) ;

    /// Install a simple message filter that always call the ConsoleMessageHandler.
    m.setAFilter( [](Message&) { return true ; }, consolehandler );


    CountingMessageHandler* countinghandler = new CountingMessageHandler() ;
    /// Install a new message filter that cal the counting message handler if the message
    /// are Runtime & Info
    m.setAFilter( [](Message& m)
        {
            if(m.context() == Message::Runtime && m.type() == Message::Warning)
                return true ;
            return false ;
        }, countinghandler );

    msg_info("test") << "An info message " ;
    dmsg_info("test") << "An info message " ;
    logmsg_info("test") << "An info message " ;

    for(auto& type : errortypes)
        EXPECT_EQ( countinghandler->getMessageCountFor(type), 0) ;

    msg_warning("test") << "An second message " ;
    dmsg_warning("test") << "An second message " ;
    logmsg_warning("test") << "An second message " ;

    EXPECT_EQ( countinghandler->getMessageCountFor(Message::Warning), 1) ;
    EXPECT_EQ( countinghandler->getMessageCountFor(Message::Info), 0) ;


    countinghandler->reset() ;
    m.removeAllFilters();

    msg_warning("test") << "An second message " ;
    dmsg_warning("test") << "An second message " ;
    logmsg_warning("test") << "An second message " ;

    EXPECT_EQ( countinghandler->getMessageCountFor(Message::Warning), 0) ;
    EXPECT_EQ( countinghandler->getMessageCountFor(Message::Info), 0) ;

    delete consolehandler ;
}
