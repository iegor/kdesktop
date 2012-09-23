#ifndef KANIMATION_H
#define KANIMATION_H

#include <qcanvas.h>
#include <qdict.h>

class QString;

/**
 * This class contains information about one frame.
 */
class KFrame
{
    public:
    /**
     * The constructor gets:
     * @param nx the position x coordinate
     * @param ny the position  coordinate
     * @param nf the frame number 
     */
    KFrame( double nx, double ny, int nf );
    /**
     * The destructor is empty yet
     */
    ~KFrame();

    double x, y; // position
    int f; // frame number
};

/**
 * This class handles a list of frames, called "scene".
 * Also a list of possibly following scene names is maintained.
 */
class KScene: public QPtrList<KFrame>
{
    public:
        /**
         * The constructor initiates the list of following scene names.
         * Attention: this list uses the autoDelete functionality as the KScene itself do.
         */
        KScene();
        /**
         * The destructor deletes the list of following scene names.
         */
        ~KScene();

        /**
         * @return a pointer to the current frame or 0, if the list is empty.
         */
        KFrame *currentFrame();
        /**
         * increases the current frame. Resets the scene, if the laste frame was reached.
         * @return a pointer to the next frame or 0, if there is no frame left or the list is empty
         */
        KFrame *nextFrame();

        /**
         * adds a scene name
         */
        void addFollowingScene( const QString scene_name );
        /**
         * selects a scene out of the list of possibly following scenes.
         */
        void followingSceneName( QString& scene_name);
        
        void setToStart();

    private:
        QStrList *following;
        unsigned int current_frame_number;
};

/**
 * KStoryBoard
 */
class KStoryBoard: public QDict<KScene>
{
    public:
        KStoryBoard( const QString filename );
        ~KStoryBoard();

        KFrame *nextFrame();
        KFrame *currentFrame();
        void setToStart();

    private:
        KScene *current_scene;
};

class KAnimation: public QCanvasSprite
{
    public:
        KAnimation( const QString stroy_filename, QCanvasPixmapArray *a, QCanvas *canvas );
        ~KAnimation();

        virtual void advance( int phase );

        void setToStart();

        KStoryBoard *story;
};

#endif
