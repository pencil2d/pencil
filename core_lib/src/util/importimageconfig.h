#ifndef IMPORTPOSITIONCONFIG_H
#define IMPORTPOSITIONCONFIG_H

struct ImportImageConfig
{
    enum PositionType {
        CenterOfView,
        CenterOfCanvas,
        CenterOfCamera,
        CenterOfCameraFollowed,
        None
    };

    int importFrame = 1;
    PositionType positionType = None;
};

#endif // IMPORTIMAGECONFIG_H
