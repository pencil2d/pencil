#include <cstdlib>
#include "AutoTest.h"
#include <QTest>

//QTEST_MAIN
int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setAttribute( Qt::AA_Use96Dpi, true );

    int ret = AutoTest::run(argc, argv);
    bool allTestsPass = ( ret == 0 );

    if ( allTestsPass )
    {
        qDebug() << "\n==== ALL TESTS PASSED ====\n";
    }
    else
    {
        qDebug() << "\n==== SOME TESTS FAILED ====\n";
    }

#ifdef _MSC_VER 
    system("PAUSE");
#endif

    return ret;
}
