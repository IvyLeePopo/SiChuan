// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� TWSDNETPAY_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// TWSDNETPAY_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef TWSDNETPAY_EXPORTS
#define TWSDNETPAY_API __declspec(dllexport)
#else
#define TWSDNETPAY_API __declspec(dllimport)
#endif

// �����Ǵ� TWSDNetPay.dll ������
class TWSDNETPAY_API CTWSDNetPay {
public:
	CTWSDNetPay(void);
	// TODO: �ڴ�������ķ�����
};

extern TWSDNETPAY_API int nTWSDNetPay;

TWSDNETPAY_API int fnTWSDNetPay(void);
