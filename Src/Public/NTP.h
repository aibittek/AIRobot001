#ifndef __NTP_H
#define __NTP_H

/**
 * @struct x_ntp_timestamp_t
 * @brief  NTP ʱ�����
 */
typedef struct x_ntp_timestamp_t
{
    unsigned int  xut_seconds;    ///< �� 1900������������������
    unsigned int  xut_fraction;   ///< С�����ݣ���λ��΢������4294.967296( = 2^32 / 10^6 )��
} x_ntp_timestamp_t;

/**
 * @enum  em_ntp_mode_t
 * @brief NTP����ģʽ�����ö��ֵ��
 */
typedef enum em_ntp_mode_t
{
    ntp_mode_unknow     = 0,  ///< δ����
    ntp_mode_initiative = 1,  ///< �����Ե���ģʽ
    ntp_mode_passive    = 2,  ///< �����Ե���ģʽ
    ntp_mode_client     = 3,  ///< �ͻ���ģʽ
    ntp_mode_server     = 4,  ///< ������ģʽ
    ntp_mode_broadcast  = 5,  ///< �㲥ģʽ���鲥ģʽ
    ntp_mode_control    = 6,  ///< ����Ϊ NTP ���Ʊ���
    ntp_mode_reserved   = 7,  ///< Ԥ�����ڲ�ʹ��
} em_ntp_mode_t;

/**
 * @struct x_ntp_packet_t
 * @brief  NTP ���ĸ�ʽ��
 */
typedef struct x_ntp_packet_t
{
    unsigned char     xct_li_ver_mode;      ///< 2 bits����Ծָʾ����3 bits���汾�ţ�3 bits��NTP����    ģʽ���ο� em_ntp_mode_t ���ö��ֵ��
    unsigned char     xct_stratum    ;      ///< ϵͳʱ�ӵĲ�����ȡֵ��ΧΪ1~16����������ʱ�ӵ�׼ȷ    �ȡ�����Ϊ1��ʱ��׼ȷ����ߣ�׼ȷ�ȴ�1��16���εݼ�������Ϊ16��ʱ�Ӵ���δͬ��״̬��������Ϊ�ο�ʱ��
    unsigned char     xct_poll       ;      ///< ��ѯʱ�䣬����������NTP����֮���ʱ����
    unsigned char     xct_percision  ;      ///< ϵͳʱ�ӵľ���

    unsigned int      xut_root_delay     ;  ///< ���ص����ο�ʱ��Դ������ʱ��
    unsigned int      xut_root_dispersion;  ///< ϵͳʱ����������ο�ʱ�ӵ�������
    unsigned int      xut_ref_indentifier;  ///< �ο�ʱ��Դ�ı�ʶ

    x_ntp_timestamp_t xtmst_reference;      ///< ϵͳʱ�����һ�α��趨����µ�ʱ��
    x_ntp_timestamp_t xtmst_originate;      ///< NTP�������뿪���Ͷ�ʱ���Ͷ˵ı���ʱ��
    x_ntp_timestamp_t xtmst_receive  ;      ///< NTP�����ĵ�����ն�ʱ���ն˵ı���ʱ��
    x_ntp_timestamp_t xtmst_transmit ;      ///< Ӧ�����뿪Ӧ����ʱӦ���ߵı���ʱ��
} x_ntp_packet_t;

extern inline unsigned long mktimes(unsigned int year, unsigned int mon, unsigned int day, unsigned int hour, unsigned int min, unsigned int sec)
{
    if (0 >= (int) (mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
        mon += 12;      /* Puts Feb last since it has leap day */
        year -= 1;
    }

    return (((
                (unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
                year*365 - 719499
            )*24 + hour /* now have hours */
        )*60 + min /* now have minutes */
    )*60 + sec; /* finally seconds */
}
//mktimes(2019, 10, 11, 3, 01, 00);

#endif
