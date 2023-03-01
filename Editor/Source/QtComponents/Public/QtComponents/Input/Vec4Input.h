#ifndef VEC4INPUT_H
#define VEC4INPUT_H

#include <QWidget>
#include <QString>

namespace Ui {
class Vec4Input;
}

namespace CE::Editor::Qt
{

    enum class Vec4Mode
    {
        Vec4,
        Vec3,
        Vec2
    };

    class IVec4InputListener
    {
    public:
        virtual void OnValueChanged(Vec4 value) {}
    };

    class QTCOMPONENTS_API Vec4Input : public QWidget
    {
        Q_OBJECT
    public:
        explicit Vec4Input(QWidget* parent = nullptr);
        ~Vec4Input();

        void SetMode(Vec4Mode mode, bool integerMode);

        void SetLabel(String label);

        void SetListener(IVec4InputListener* listener)
        {
            this->listener = listener;
        }

        void SetValue(Vec4 value);
        void SetValue(Vec4i value);

        Vec4Mode GetVecMode() { return vecMode; }

        bool IsIntegerMode() { return integerMode; }

    signals:
        void OnValueChanged(f32 x, f32 y, f32 z, f32 w);

    private slots:
        void on_inputX_textChanged(const QString &arg1);

        void on_inputY_textChanged(const QString &arg1);

        void on_inputZ_textChanged(const QString &arg1);

        void on_inputW_textChanged(const QString &arg1);

        void on_inputX_textEdited(const QString &arg1);

        void on_inputY_textEdited(const QString &arg1);

        void on_inputZ_textEdited(const QString &arg1);

        void on_inputW_textEdited(const QString &arg1);

    private:
        void SignalValueChange();
        void ParseFloat(QString string, f32& outFloat);

    private:
        IVec4InputListener* listener = nullptr;

        Vec4Mode vecMode{};
        bool integerMode = false;
        Ui::Vec4Input* ui;
    };

}

#endif // VEC4INPUT_H
