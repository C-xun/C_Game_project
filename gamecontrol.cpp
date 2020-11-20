#include "GameControl.h"
#include <QMetaType>

#include "Hand.h"
#include "gamemusic.h"

#include <QMessageBox>
#include <QtNetwork/QAbstractSocket>
#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <QDateTime>

#define SETTINGS_FILE qApp->applicationDirPath() + "/score.ini"

GameControl::GameControl(QObject* parent)
    : QObject(parent)
{
    qRegisterMetaType<Hand>("Hand");
    qRegisterMetaType<Card>("Card");
    qRegisterMetaType<Cards>("Cards");

    m_punchPlayer = NULL;
    m_punchCards = Cards();

    m_curBet = 0;
}

GameControl::~GameControl()
{

}

void GameControl::Init()
{
    m_robotLeft = new Robot(this);
    m_robotRight = new Robot(this);
    m_user = new UserPlayer(this);

    m_robotLeft->SetName(tr("左边机器人"));
    m_robotRight->SetName(tr("右边机器人"));
    m_user->SetName(tr("打牌小能手"));

    // 与三个玩家对接
    connect(m_robotLeft, SIGNAL(NotifyCallLord(Player*, int)), this, SLOT(OnPlayerCalledLord(Player*, int)));
    connect(m_robotRight, SIGNAL(NotifyCallLord(Player*, int)), this, SLOT(OnPlayerCalledLord(Player*, int)));
    connect(m_user, SIGNAL(NotifyCallLord(Player*, int)), this, SLOT(OnPlayerCalledLord(Player*, int)));

    connect(m_robotLeft, SIGNAL(NotifyPlayHand(Player*, const Cards&)), this, SLOT(OnPlayerPlayedHand(Player*, const Cards&)));
    connect(m_robotRight, SIGNAL(NotifyPlayHand(Player*, const Cards&)), this, SLOT(OnPlayerPlayedHand(Player*, const Cards&)));
    connect(m_user, SIGNAL(NotifyPlayHand(Player*, const Cards&)), this, SLOT(OnPlayerPlayedHand(Player*, const Cards&)));

    connect(m_robotLeft, SIGNAL(NotifyPickCards(Player*, const Cards&)), this, SLOT(OnPlayerPickCards(Player*, const Cards&)));
    connect(m_robotRight, SIGNAL(NotifyPickCards(Player*, const Cards&)), this, SLOT(OnPlayerPickCards(Player*, const Cards&)));
    connect(m_user, SIGNAL(NotifyPickCards(Player*, const Cards&)), this, SLOT(OnPlayerPickCards(Player*, const Cards&)));

    connect(this, SIGNAL(NotifyPlayerPunch(Player*, const Cards&)),
        m_robotLeft, SLOT(OnPlayerPunch(Player*, const Cards&)));
    connect(this, SIGNAL(NotifyPlayerPunch(Player*, const Cards&)),
        m_robotRight, SLOT(OnPlayerPunch(Player*, const Cards&)));
    connect(this, SIGNAL(NotifyPlayerPunch(Player*, const Cards&)),
        m_user, SLOT(OnPlayerPunch(Player*, const Cards&)));

    m_robotLeft->SetPrevPlayer(m_robotRight);
    m_robotLeft->SetNextPlayer(m_user);
    m_robotRight->SetPrevPlayer(m_user);
    m_robotRight->SetNextPlayer(m_robotLeft);
    m_user->SetPrevPlayer(m_robotLeft);
    m_user->SetNextPlayer(m_robotRight);

    m_curPlayer = m_user;

    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.beginGroup("Score");
    m_user->SetMark(settings.value("User", 0).toInt());
    m_robotLeft->SetMark(settings.value("LeftRobot", 0).toInt());
    m_robotRight->SetMark(settings.value("RightRobot", 0).toInt());
    settings.endGroup();
}

void GameControl::AddCard(CardSuit suit, CardPoint point)
{
    Card card;
    card.suit = suit;
    card.point = point;

    m_allCards.Add(card);
}

void GameControl::InitCards()
{
    m_allCards.Clear();

    for (int p = Card_Begin + 1; p < Card_SJ; p++)
    {
        for (int s = Suit_Begin + 1; s < Suit_End; s++)
        {
            AddCard((CardSuit)s, (CardPoint)p);
        }
    }

    AddCard(Suit_Begin, Card_SJ);
    AddCard(Suit_Begin, Card_BJ);
}

void GameControl::ShuffleCards()
{

}

void GameControl::StartCallLord()
{
    m_betList.clear();

    emit NotifyPlayerStatus(m_curPlayer, GameControl::ThinkingForCallLordStatus);
    m_curPlayer->StartCallLord();
}

void GameControl::OnPlayerCalledLord(Player* player, int bet)
{
    NotifyPlayerCalledLord(player, bet);

    if (bet == 3)	// 叫3分，直接当地主
    {
        m_curBet = 3;

        player->SetRole(Player::Lord);
        player->GetPrevPlayer()->SetRole(Player::Farmer);
        player->GetNextPlayer()->SetRole(Player::Farmer);

        m_curPlayer = player;
        player->PickCards(m_allCards);
        m_allCards.Clear();

        // 延迟一下，开始出牌过程
        QTimer::singleShot(1000, this, SLOT(StartPlayHand()));

        return;
    }

    BetRecrod record;
    record.player = player;
    record.bet = bet;
    m_betList.push_back(record);

    // 根据叫分情况划分地主农民角色
    if (m_betList.size() == 3)	// 三人都叫了一遍
    {
        // 三人叫分一样，重新发牌
        if (m_betList[0].bet == m_betList[1].bet &&
            m_betList[1].bet == m_betList[2].bet)
        {
            emit NotifyGameStatus(PickingCardStatus);
        }
        else
        {
            BetRecrod record;
            if (m_betList[0].bet >= m_betList[1].bet)
            {
                record = m_betList[0];
            }
            else
            {
                record = m_betList[1];
            }

            if (record.bet < m_betList[2].bet)
            {
                record = m_betList[2];
            }

            record.player->SetRole(Player::Lord);
            record.player->GetPrevPlayer()->SetRole(Player::Farmer);
            record.player->GetNextPlayer()->SetRole(Player::Farmer);
            m_curPlayer = record.player;

            m_curBet = record.bet;

            m_curPlayer->PickCards(m_allCards);
            m_allCards.Clear();

            QTimer::singleShot(1000, this, SLOT(StartPlayHand()));
        }

        return;
    }

    // 让下一个玩家叫分
    Player* nextPlayer = player->GetNextPlayer();
    m_curPlayer = nextPlayer;
    emit NotifyPlayerStatus(m_curPlayer, GameControl::ThinkingForCallLordStatus);
    m_curPlayer->StartCallLord();
}

void GameControl::StartPlayHand()
{
    emit NotifyGameStatus(PlayingHandStatus);

    emit NotifyPlayerStatus(m_curPlayer, GameControl::ThinkingForPlayHandStatus);
    m_curPlayer->StartPlayHand();
}

// 出牌
void GameControl::OnPlayerPlayedHand(Player* player, const Cards& cards)
{
    if (!cards.IsEmpty())
    {
        m_punchPlayer = player;
        m_punchCards = cards;

        NotifyPlayerPunch(m_punchPlayer, m_punchCards);
    }

    // 有炸，底分翻倍
    Hand hand(cards);
    HandType handType = hand.GetType();
    if (handType == Hand_Bomb || handType == Hand_Bomb_Jokers)
    {
        m_curBet = m_curBet * 2;
    }
    //
    emit NotifyPlayerPlayHand(m_curPlayer, cards);

    // player已把牌都出完，计算三方得分
    if (player->GetCards().IsEmpty())
    {
        Player* prev = player->GetPrevPlayer();
        Player* next = player->GetNextPlayer();

        if (player->GetRole() == Player::Lord)
        {
            player->SetMark(player->GetMark() + 2 * m_curBet);
            prev->SetMark(prev->GetMark() - m_curBet);
            next->SetMark(next->GetMark() - m_curBet);
        }
        else
        {
            player->SetMark(player->GetMark() + m_curBet);

            if (prev->GetRole() == Player::Lord)
            {
                prev->SetMark(prev->GetMark() - 2 * m_curBet);
                next->SetMark(next->GetMark() + m_curBet);
            }
            else
            {
                prev->SetMark(prev->GetMark() + m_curBet);
                next->SetMark(next->GetMark() - 2 * m_curBet);
            }
        }

        emit NotifyPlayerStatus(player, GameControl::WinningStatus);
        return;
    }

    // 播放音效
    PlayCardsMusic(hand.GetType(), hand.GetBasePoint());

    // 出完牌，轮到下一个玩家
    m_curPlayer = player->GetNextPlayer();
    emit NotifyPlayerStatus(m_curPlayer, GameControl::ThinkingForPlayHandStatus);
    m_curPlayer->StartPlayHand();
}

void GameControl::OnPlayerPickCards(Player* player, const Cards& cards)
{
    emit NotityPlayerPickCards(player, cards);
}

void GameControl::Clear()
{
    InitCards();

    m_robotLeft->ClearCards();
    m_robotRight->ClearCards();
    m_user->ClearCards();

    m_punchPlayer = NULL;
    m_punchCards.Clear();
    NotifyPlayerPunch(m_punchPlayer, m_punchCards);

    m_betList.clear();
}

Player* GameControl::GetUserPlayer()
{
    return m_user;
}

Player* GameControl::GetLeftRobot()
{
    return m_robotLeft;
}

Player* GameControl::GetRightRobot()
{
    return m_robotRight;
}

Player* GameControl::GetCurrentPlayer()
{
    return m_curPlayer;
}

void GameControl::SetCurrentPlayer(Player* curPlayer)
{
    m_curPlayer = curPlayer;
}

Cards GameControl::GetRestCards()
{
    return m_allCards;
}

Card GameControl::PickOneCard()
{
    Card card = m_allCards.TakeRandomCard();
    return card;
}

Player* GameControl::GetPunchPlayer()
{
    return m_punchPlayer;
}

Cards GameControl::GetPunchCards()
{
    return m_punchCards;
}

int GameControl::GetCurBet()
{
    return m_curBet;
}

void GameControl::SetCurBet(int curBet)
{
    m_curBet = curBet;
}

void GameControl::ClearPlayerMarks()
{
    m_robotLeft->SetMark(0);
    m_robotRight->SetMark(0);
    m_user->SetMark(0);
}

// 播放出牌的声音
void GameControl::PlayCardsMusic(HandType nType, CardPoint nPoint)
{
    QString strSound = "Special_give";

    switch (nType) {
    case Hand_Pass:
    {
        qsrand(QTime::currentTime().msec() * 1000);
        strSound = QString("Man_buyao%1").arg((qrand() % 4) + 1);
    }
        break;
    case Hand_Single: 			// 单牌
    {
        if (nPoint < Card_A && nPoint > Card_Begin) {
            strSound = "Man_" + QString::number(nPoint + 2);
        }
        else if (Card_A == nPoint || Card_2 == nPoint) {
             strSound = "Man_" + QString::number(nPoint - Card_K);
        }
        else if (Card_SJ == nPoint || Card_BJ == nPoint) {
            strSound = "Man_" + QString::number(nPoint);
       }
    }
        break;
    case Hand_Pair:				// 对子
    {
        if (nPoint < Card_A && nPoint > Card_Begin) {
            strSound = "Man_dui" + QString::number(nPoint + 2);
        }
        else if (Card_A == nPoint || Card_2 == nPoint) {
             strSound = "Man_dui" + QString::number(nPoint - Card_K);
        }
    }
        break;
    case Hand_Triple:			// 三个
    {
        if (nPoint < Card_A && nPoint > Card_Begin) {
            strSound = "Man_tuple" + QString::number(nPoint + 2);
        }
        else if (Card_A == nPoint || Card_2 == nPoint) {
             strSound = "Man_tuple" + QString::number(nPoint - Card_K);
        }
    }
        break;
    case Hand_Triple_Single:		// 三带一
    {
        strSound = "Man_sandaiyi";
    }
        break;
    case Hand_Triple_Pair:		// 三带二
    {
        strSound = "Man_sandaiyidui";
    }
        break;
    case Hand_Plane:				// 飞机，555_666
    {
        strSound = "Man_feiji";
    }
        break;
    case Hand_Plane_Two_Single:	// 飞机带单，555_666_3_4
    {
        strSound = "Man_feiji";
    }
        break;
    case Hand_Plane_Two_Pair:	// 飞机带双，555_666_33_44
    {
        strSound = "Special_plane";
    }
        break;
    case Hand_Seq_Pair:			// 连对，33_44_55(_66...)
    {
        strSound = "Man_liandui";
    }
        break;
    case Hand_Seq_Single:		// 顺子，34567(8...)
    {
        strSound = "Man_shunzi";
    }
        break;
    case Hand_Bomb:				// 炸弹
    {
        strSound = 0 == qrand() % 2 ? "Special_Bomb" : "Man_zhadan";
    }
        break;
    case Hand_Bomb_Single:		// 炸弹带一个
    {

    }
        break;
    case Hand_Bomb_Pair:		// 炸弹带一对
    {

    }
        break;
    case Hand_Bomb_Two_Single:	// 炸弹带两单
    {
//        strSound = "";
    }
        break;
    case Hand_Bomb_Jokers:			// 王炸
    {
        strSound = "Man_wangzha";
    }
        break;
    default:    break;
    }

    // 播放音效
    GameMusic::Instance()->play(strSound);
}
