/* BtcTradeRobot, BtcTradeRobotUnitTest */
use BtcTradeRobotUnitTest;

delete from `debitandcreditdetailinfor`;
delete from `debitandcreditbaseinfor`;

set @id1 = 1;
set @id2 = 1;
insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-14 21:34:00'), 4801675);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   492, 5719.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  6302,   65.71, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 14814,   27.02, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.64, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-14 21:50:00'), 4914730);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   492, 5615.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  6310,   65.34, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 15214,   26.65, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.59, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-14 22:24:00'), 4933971);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   511, 5660.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  6310,   65.53, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 15214,   26.80, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.59, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-14 22:28:00'), 4933971);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   530, 5663.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  6310,   65.53, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 15214,   26.80, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.59, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-14 22:28:00'), 4935971);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   530, 5672.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  7625,   65.61, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 15214,   26.82, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.59, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-14 23:00:00'), 4952976);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   550, 5649.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  7625,   65.44, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 15214,   26.82, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.59, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-14 23:54:00'), 5318076);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   574, 5711.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  7625,   65.61, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 15619,   26.95, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.59, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 10:07:00'), 6369512);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   664, 5578.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8533,   66.13, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 15619,   26.70, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.57, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 10:30:00'), 6408846);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   689, 5557.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8533,   66.23, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 16419,   26.67, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.57, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 13:20:00'), 6572416);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   708, 5576.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8533,   66.25, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 18019,   26.66, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.57, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 14:00:00'), 6625870);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   709, 5512.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8533,   66.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 19277,   26.34, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 14:17:00'), 6640870);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   709, 5506.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8533,   66.19, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 19277,   26.31, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 14:55:00'), 6671585);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   730, 5536.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8533,   66.32, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 19277,   26.40, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 16:13:00'), 6818187);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   753, 5511.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8533,   65.76, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 19277,   26.30, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 19:18:00'), 6912709);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   765, 5565.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8533,   66.60, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 21079,   26.40, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 20:32:00'), 6972709);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   767, 5596.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8533,   66.54, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 21079,   26.44, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 21:55:00'), 7162006);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   769, 5578.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8533,   66.61, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 21081,   26.54, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 22:08:00'), 7164506);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   774, 5553.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8853,   66.61, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 21081,   26.39, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.66, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 22:31:00'), 7169571);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   781, 5539.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8853,   66.39, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 21081,   26.36, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   555,    9.65, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-15 23:07:00'), 7224751);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   785, 5545.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8853,   66.46, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 21081,   26.39, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  1555,    9.65, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 08:49:00'), 7597399);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   814, 5578.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8853,   66.26, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 21881,   26.46, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.67, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 09:23:00'), 7752399);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   823, 5588.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8853,   66.11, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 21881,   26.49, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.80, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 10:05:00'), 7779366);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   830, 5683.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8853,   66.52, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23685,   26.59, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.60, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 10:44:00'), 7793156);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   830, 5683.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8853,   66.33, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23685,   26.58, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.60, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 11:09:00'), 8032569);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   847, 5685.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8853,   66.11, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23734,   26.55, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.57, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 11:59:00'), 8102141);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   849, 5688.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8853,   66.21, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23734,   26.62, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.57, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 12:46:00'), 8129276);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   849, 5688.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8943,   66.10, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23734,   26.62, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.62, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 13:06:00'), 8186920);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   850, 5664.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8943,   66.19, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23784,   26.60, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.57, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 13:45:00'), 8317637);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   850, 5668.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8943,   66.12, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23784,   26.60, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.57, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 15:37:00'), 8480187);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   872, 5692.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8943,   66.05, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23784,   26.59, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.61, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 16:15:00'), 8532147);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   886, 5674.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8943,   66.09, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23784,   26.55, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.61, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 16:50:00'), 8615506);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   896, 5672.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8943,   66.99, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23784,   26.55, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.67, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 16:54:00'), 8807684);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   896, 5686.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8943,   66.99, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23784,   26.55, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.67, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-16 19:45:00'), 8834826);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   910, 5627.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  8943,   65.82, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 23784,   26.38, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2476,    9.56, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 08:49:00'), 9085709);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   924, 5705.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  9544,   65.53, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 26784,   26.50, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.65, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 11:24:00'), 9433562);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   928, 5788.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10031,   65.17, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 26935,   26.57, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.60, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 12:04:00'), 9436020);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   949, 5845.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10031,   66.65, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 26935,   26.70, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.60, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 12:50:00'), 9818607);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   955, 5881.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10041,   66.99, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 26945,   26.72, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.68, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 14:05:00'), 9920864);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   959, 5918.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10155,   66.81, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 27143,   26.77, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.68, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 15:12:00'), 10202926);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   985, 5966.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10155,   66.08, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 27143,   26.80, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.68, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 16:04:00'), 10208304);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1055, 6086.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10155,   67.88, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 27143,   27.10, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.75, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 16:21:00'), 10208304);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1078, 6164.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10155,   68.29, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 29719,   27.18, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.75, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 16:30:00'), 10330677);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1080, 6112.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10155,   67.91, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 29719,   27.07, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.75, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 16:50:00'), 10430677);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1105, 6092.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10155,   67.81, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 29719,   26.78, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.75, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 19:16:00'), 10498242);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1121, 6133.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10207,   68.32, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 29719,   27.08, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.63, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-17 20:34:00'), 10562079);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1122, 6095.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10207,   69.20, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 29719,   26.97, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.63, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-18 08:49:00'), 11121012);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1179, 6323.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10287,   72.50, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 30171,   27.14, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.66, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-18 08:49:00'), 11121012);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1179, 6323.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10287,   72.50, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 30171,   27.14, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.66, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-18 08:49:00'), 11121012);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1179, 6323.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10287,   72.50, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 30171,   27.14, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.66, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-18 11:08:00'), 11315936);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1192, 6154.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10317,   72.20, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 30171,   26.85, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-18 11:33:00'), 11315936);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1210, 6155.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10317,   72.29, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 30171,   26.81, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-18 12:29:00'), 11352096);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1231, 6176.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10317,   70.87, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 30171,   26.86, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-18 15:37:00'), 11579461);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1237, 6159.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10317,   70.80, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 30171,   26.76, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-18 16:04:00'), 11579461);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1252, 6147.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 10317,   70.26, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 30171,   26.70, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-18 21:53:00'), 11714115);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1283, 5842.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 11076,   67.21, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 36148,   26.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-18 22:14:00'), 12033715);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',  1301, 5778.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth', 11076,   66.90, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc', 36148,   25.85, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',  2555,    9.38, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-19 09:12:00'), 12266730);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   1336, 6035.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  11076,   69.08, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc',  37840,   26.17, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   2555,    9.46, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-19 11:23:00'), 12266730);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   1341, 6064.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  11076,   69.28, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc',  38040,   26.25, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   2555,    9.50, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-19 11:41:00'), 12266730);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   1397, 6075.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  11076,   69.70, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc',  38040,   26.31, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   2555,    9.45, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-19 15:45:00'), 12266812);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   1403, 6054.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  11076,   69.80, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc',  38040,   26.14, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   2555,    9.43, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-19 20:51:00'), 12335903);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   1403, 6103.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  11162,   69.62, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc',  38040,   26.24, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   2555,    9.49, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-20 13:52:00'), 12612077);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   1420, 6106.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  11164,   72.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc',  38040,   26.48, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   2555,    9.33, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-20 23:42:00'), 12721169);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   1422, 6132.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  11164,   72.46, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc',  38040,   26.70, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   2555,    9.22, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-24 10:24:00'), 14451427);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   1908, 6310.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  11214,   73.13, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc',  38488,   26.32, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   3032,    9.05, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;

insert into `debitandcreditbaseinfor` values (@id1, UNIX_TIMESTAMP('2017-01-25 21:49:00'), 14843787);
insert into `debitandcreditdetailinfor` values (@id2, 'Btc',   2000, 6188.00, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Eth',  11738,   71.95, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ltc',  39699,   25.81, @id1);
set @id2 = @id2 + 1;
insert into `debitandcreditdetailinfor` values (@id2, 'Ybc',   3032,    9.05, @id1);
set @id2 = @id2 + 1;
set @id1 = @id1 + 1;


/*
select * from DebitAndCreditlView where CoinType = 'Btc';

select convert((max(LendableMoney) - min(LendableMoney)) / avg(Price), decimal(10, 2)) as SellCoinNumber, 
       (max(LendableCoinNumber) - min(LendableCoinNumber)) as BuyCoinNumber 
from `DebitAndCreditlView` 
where CoinType = 'Btc' 
    and UNIX_TIMESTAMP(DateTime) > UNIX_TIMESTAMP('2017-01-10 12:00:00');
*/