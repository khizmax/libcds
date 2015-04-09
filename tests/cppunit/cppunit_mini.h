//$$CDS-header$$

/*
 * Copyright (c) 2003, 2004
 * Zdenek Nemec
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

/*
    Partially changed and expanded by Maxim Khiszinsky (cds), 2009
*/

/* $Id$ */

#ifndef CDS_CPPUNIT_MPFR_H_
#define CDS_CPPUNIT_MPFR_H_

#include <string.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <random>

#include <assert.h>

#include <boost/lexical_cast.hpp>

namespace CppUnitMini
{
  class Reporter {
  public:
    virtual ~Reporter() {}
    virtual void error(const char * /*macroName*/, const char * /*in_macro*/, const char * /*in_file*/, int /*in_line*/) {}
    virtual void message( const char * /*msg*/ ) {}
    virtual void progress( const char * /*in_className*/, const char * /*in_testName*/, bool /*ignored*/, bool /* explicit */) {}
    virtual void end() {}
    virtual void printSummary() {}
  };

  struct TestCfg
  {
      typedef std::map< std::string, std::string >  cfg_map;
      cfg_map    m_Cfg ; // map param_name => value

      template <typename T>
      T get( const std::string& strParamName, T defVal ) const
      {
          cfg_map::const_iterator it = m_Cfg.find( strParamName );
          if ( it == m_Cfg.end() )
              return defVal ; // param not found -> returns default value
          try {
              return boost::lexical_cast< T >( it->second );
          }
          catch ( boost::bad_lexical_cast& ex )
          {
              std::cerr << "bad_lexical_cast encountered while getting parameter "
                  << strParamName << "=" << it->second
                  << ": " << ex.what()
                  << std::endl
;
          }
          return defVal;
      }

      template <typename T>
      T get( const char * pszParamName, T defVal ) const
      {
          return get( std::string( pszParamName ), defVal );
      }

      int getInt( const char * pszParamName, int nDefVal = 0 ) const { return get( pszParamName, nDefVal ) ; }
      unsigned int getUInt( const char * pszParamName, unsigned int nDefVal = 0 ) const { return get( pszParamName, nDefVal ) ; }
      long getLong( const char * pszParamName, long nDefVal = 0 ) const { return get( pszParamName, nDefVal ) ; }
      unsigned long getULong( const char * pszParamName, unsigned long nDefVal = 0 ) const { return get( pszParamName, nDefVal ) ; }
      size_t getSizeT( const char * pszParamName, size_t nDefVal = 0 ) const 
      {
          return static_cast<size_t>( getULong( pszParamName, static_cast<unsigned long>(nDefVal)));
      }

      bool getBool( const char * pszParamName, bool bDefVal = false ) const
      {
          std::string strParamName( pszParamName );
          cfg_map::const_iterator it = m_Cfg.find( strParamName );
          if ( it == m_Cfg.end() )
              return bDefVal ; // param not found -> returns default value
          try {
              return boost::lexical_cast< int >( it->second ) != 0;
          }
          catch ( boost::bad_lexical_cast& ex )
          {
              std::cerr << "bad_lexical_cast encountered while getting parameter "
                  << strParamName << "=" << it->second
                  << ": " << ex.what()
                  << std::endl;
          }
          return bDefVal;
      }

  };

  class Config {
      std::map< std::string, TestCfg>  m_Cfg;

  public:
      Config() {}

      void load( const char * fileName );

      TestCfg& get( const std::string& strTestName )
      {
          return m_Cfg[ strTestName ];
      }
  };

  class TestFixture {
  public:
    virtual ~TestFixture() {}

    //! \brief Set up context before running a test.
    virtual void setUp() {}

    //! Clean up after the test run.
    virtual void tearDown() {}
  };

  class TestCase : public TestFixture {
  public:
    TestCase() { registerTestCase(this); }

    void setUp() { m_failed = false; }
    static int run(Reporter *in_reporter = 0, const char *in_testName = "", bool invert = false);
    int numErrors() { return m_numErrors; }
    static void registerTestCase(TestCase *in_testCase);

    static TestCase * current_test()
    {
        assert( m_pCurTestCase );
        return m_pCurTestCase;
    }

    virtual void setUpParams( const TestCfg& /*cfg*/ ) {}
    virtual void endTestCase() {}
    virtual void myRun(const char * /*in_name*/, bool /*invert*/ = false) {}

    virtual void error(const char *in_macroName, const char *in_macro, const char *in_file, int in_line) {
      m_failed = true;
      if (m_reporter) {
        m_reporter->error(in_macroName, in_macro, in_file, in_line);
      }
    }

    static void message(const char *msg) {
      if (m_reporter) {
        m_reporter->message(msg);
      }
    }

    bool equalDoubles(double in_expected, double in_real, double in_maxErr) {
      double diff = in_expected - in_real;
      if (diff < 0.) {
        diff = -diff;
      }
      return diff < in_maxErr;
    }

    virtual void progress(const char *in_className, const char *in_functionName, bool ignored, bool explicitTest) {
      ++m_numTests;
      if (m_reporter) {
        m_reporter->progress(in_className, in_functionName, ignored, explicitTest);
      }
    }

    bool shouldRunThis( const char *in_desiredTest, const char *in_className, const char *in_functionName,
                        bool invert, bool explicit_test, bool &do_progress );

    void tearDown() {
      print_gc_state();
      if (m_failed)
        ++m_numErrors;
      m_reporter->end();
    }

    static void print_gc_state();

    static std::vector<std::string> const&    getTestStrings();

    template <typename RandomIt>
    static void shuffle( RandomIt first, RandomIt last )
    {
        std::shuffle( first, last, m_RandomGen );
    }

  protected:
    static std::vector<std::string>  m_arrStrings ;   // array of test strings

  public:
      static bool m_bPrintGCState   ;   // print GC state after each test
      static Config m_Cfg;
      static std::string m_strTestDataDir;
      static bool m_bExactMatch;

      // random shuffle support
      static std::random_device m_RandomDevice;
      static std::mt19937       m_RandomGen;

  protected:
    static int m_numErrors;
    static int m_numTests;

    static TestCase * m_pCurTestCase;

  private:
    static TestCase *m_root;
    TestCase *m_next;
    bool m_failed;

    static Reporter *m_reporter;
  };

}

#if !defined (CPPUNIT_MINI_HIDE_UNUSED_VARIABLE)
#  if defined (_MSC_VER)
#    define CPPUNIT_MINI_HIDE_UNUSED_VARIABLE(v) (v);
#  else
#    define CPPUNIT_MINI_HIDE_UNUSED_VARIABLE(v)
#  endif
#endif

#define CPPUNIT_TEST_SUITE_(X, cfgBranchName) \
    typedef CppUnitMini::TestCase Base; \
    virtual void myRun(const char *in_name, bool invert = false) { \
    const char *className = #X; CPPUNIT_MINI_HIDE_UNUSED_VARIABLE(className) \
    bool ignoring = false; CPPUNIT_MINI_HIDE_UNUSED_VARIABLE(ignoring) \
    setUpParams( m_Cfg.get( cfgBranchName ));

#define CPPUNIT_TEST_SUITE_PART(X, func) \
    void X::func(const char *in_name, bool invert /*= false*/) { \
    const char *className = #X; CPPUNIT_MINI_HIDE_UNUSED_VARIABLE(className) \
    bool ignoring = false; CPPUNIT_MINI_HIDE_UNUSED_VARIABLE(ignoring)

#define CPPUNIT_TEST_SUITE(X) CPPUNIT_TEST_SUITE_(X, #X)

#if defined CPPUNIT_MINI_USE_EXCEPTIONS
#  define CPPUNIT_TEST_BASE(X, Y) \
  { \
    bool do_progress; \
    bool shouldRun = shouldRunThis(in_name, className, #X, invert, Y, do_progress); \
    if (shouldRun || do_progress) { \
      setUp(); \
      progress(className, #X, ignoring || !shouldRun, !ignoring && Y); \
      if (shouldRun && !ignoring) { \
        try { \
          X(); \
        } \
        catch(...) { \
          Base::error("Test Failed: An exception was thrown.", #X, __FILE__, __LINE__); \
        } \
      } \
      tearDown(); \
    } \
  }
#else
#  define CPPUNIT_TEST_BASE(X, Y) \
  { \
    bool do_progress; \
    bool shouldRun = shouldRunThis(in_name, className, #X, invert, Y, do_progress); \
    if (shouldRun || do_progress) { \
      setUp(); \
      progress(className, #X, ignoring || !shouldRun, !ignoring && Y); \
      if (shouldRun && !ignoring) \
        X(); \
      tearDown(); \
    } \
  }
#endif

#define CPPUNIT_TEST(X) CPPUNIT_TEST_BASE(X, false)
#define CPPUNIT_EXPLICIT_TEST(X) CPPUNIT_TEST_BASE(X, true)

#define CDSUNIT_DECLARE_TEST(X) void X();

#define CPPUNIT_IGNORE \
  ignoring = true

#define CPPUNIT_STOP_IGNORE \
  ignoring = false

#define CPPUNIT_TEST_SUITE_END() endTestCase(); }
#define CPPUNIT_TEST_SUITE_END_PART() }

#define CPPUNIT_TEST_SUITE_REGISTRATION(X) static X local
#define CPPUNIT_TEST_SUITE_REGISTRATION_(X, NAME) static X NAME

#define CPPUNIT_CHECK(X) \
  if (!(X)) { \
    Base::error("CPPUNIT_CHECK", #X, __FILE__, __LINE__); \
  }

#define CPPUNIT_CHECK_CURRENT(X) \
  if (!(X)) { \
    CppUnitMini::TestCase::current_test()->error("CPPUNIT_CHECK", #X, __FILE__, __LINE__); \
  }

#define CPPUNIT_CHECK_EX(X, Y) \
    if (!(X)) { \
        std::stringstream st    ;   \
        st << #X << ": " << Y   ;   \
        Base::error("CPPUNIT_CHECK", st.str().c_str(), __FILE__, __LINE__); \
    }

#define CPPUNIT_CHECK_CURRENT_EX(X, Y) \
    if (!(X)) { \
        std::stringstream st    ;   \
        st << #X << ": " << Y   ;   \
        CppUnitMini::TestCase::current_test()->error("CPPUNIT_CHECK", st.str().c_str(), __FILE__, __LINE__); \
    }

#define CPPUNIT_ASSERT(X) \
  if (!(X)) { \
    Base::error("CPPUNIT_ASSERT", #X, __FILE__, __LINE__); \
    return; \
  }

#define CPPUNIT_ASSERT_CURRENT(X) \
    if (!(X)) { \
        CppUnitMini::TestCase::current_test()->error("CPPUNIT_ASSERT", #X, __FILE__, __LINE__); \
        return; \
    }


#define CPPUNIT_ASSERT_EX(A, X) \
    if (!(A)) { \
        std::stringstream st    ;   \
        st << #A << ": " << X   ;   \
        Base::error("CPPUNIT_ASSERT", st.str().c_str(), __FILE__, __LINE__); \
        return; \
    }

#define CPPUNIT_FAIL { \
    Base::error("CPPUNIT_FAIL", "", __FILE__, __LINE__); \
    return; \
  }

#define CPPUNIT_ASSERT_EQUAL(X, Y) \
  if ((X) != (Y)) { \
    Base::error("CPPUNIT_ASSERT_EQUAL", #X","#Y, __FILE__, __LINE__); \
    return; \
  }

#define CPPUNIT_ASSERT_DOUBLES_EQUAL(X, Y, Z) \
  if (!equalDoubles((X), (Y), (Z))) { \
    Base::error("CPPUNIT_ASSERT_DOUBLES_EQUAL", #X","#Y","#Z, __FILE__, __LINE__); \
    return; \
  }

// added by cds
#define CPPUNIT_MSG( X ) \
    {   \
        std::stringstream st    ;   \
        st << X ;   \
        if ( !st.str().empty() ) \
            CppUnitMini::TestCase::message( st.str().c_str() );   \
    }

#define CPPUNIT_MESSAGE(m) CppUnitMini::TestCase::message(m)

#define CPPUNIT_ASSERT_MSG( A, X ) \
    if ( !(A) ){   \
        std::stringstream st    ;   \
        st << #A << ": " << X ;   \
        error( "CPPUNIT_ASSERT_MSG", st.str().c_str(), __FILE__, __LINE__ )     ;   \
    }

#endif // #ifndef CDS_CPPUNIT_MPFR_H_
