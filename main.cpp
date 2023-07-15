#include "mainwindow.h"
#include <qgsapplication.h>
#include <gdal_priv.h>

//#define ENABLE_DEBUG_OUTPUT

int main(int argc, char *argv[])
{
//    const char* version = GDALVersionInfo("RELEASE_NAME");

//    if (version != nullptr) {
//        std::cout << "GDAL Version: " << version << std::endl;
//    }
//    else {
//        std::cout << "Failed to get GDAL version." << std::endl;
//    }
    GDALAllRegister();
    CPLSetConfigOption("GDAL_PAM_ENABLED", "NO");
#ifdef ENABLE_DEBUG_OUTPUT
    std::cout << "Enable Debug Output" << std::endl;
#endif

    QgsApplication app(argc, argv, true);
    MainWindow main_window("C:/Users/eric/Desktop/qtCode/");

    main_window.show();

    return app.exec();

//    miniz_cpp::zip_file file;
//    //file.write("E:/QtProject/Proj0/main.cpp");
//    file.write("C:/Users/eric/Desktop/qtCode/111.tif");
//    file.write("C:/Users/eric/Desktop/qtCode/111.xml");
//    file.save("C:/Users/eric/Desktop/qtCode/aaa.zip");

    return 0;
}
