#ifndef COMMANDCENTER_H
#define COMMANDCENTER_H

#include <QObject>

class Editor;


class CommandCenter : public QObject
{
    Q_OBJECT

public:
    explicit CommandCenter(QObject* parent = 0);
    virtual ~CommandCenter();

    void setCore( Editor* e ) { mEditor = e; }
    // view
    void ZoomIn();
    void ZoomOut();

    // Animation
    void GotoNextFrame();
    void GotoPrevFrame();
    void GotoNextKeyFrame();
    void GotoPrevKeyFrame();

private:
    Editor* mEditor = nullptr;
};

#endif // COMMANDCENTER_H
