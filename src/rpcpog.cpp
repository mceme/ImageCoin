// Copyright (c) 2014-2019 The Dash Core Developers, The DAC Core Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "rpcpog.h"
#include "spork.h"
#include "util.h"
#include "utilmoneystr.h"
#include "rpcpodc.h"
#include "init.h"
#include "bbpsocket.h"
#include "activemasternode.h"
#include "governance-classes.h"
#include "governance.h"
#include "masternode-sync.h"
#include "masternode-payments.h"
#include "messagesigner.h"
#include "smartcontract-server.h"
#include "smartcontract-client.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp> // for to_lower()
#include <boost/algorithm/string.hpp> // for trim()
#include <boost/date_time/posix_time/posix_time.hpp> // for StringToUnixTime()
#include <math.h>       /* round, floor, ceil, trunc */
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <openssl/crypto.h>
#include <stdint.h>
#include <univalue.h>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <openssl/md5.h>
#include "txmempool.h"
// For HTTPS (for the pool communication)
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <boost/asio.hpp>
#include "net.h" // for CService
#include "netaddress.h"
#include "netbase.h" // for LookupHost
#include "wallet/wallet.h"
#include <sstream>
//#include "randomx_bbp.h"

#ifdef ENABLE_WALLET
extern CWallet* pwalletMain;

#endif // ENABLE_WALLET

CValidationState state;

UniValue VoteWithMasternodes(const std::map<uint256, CKey>& keys,	
                             const uint256& hash, vote_signal_enum_t eVoteSignal,	
                             vote_outcome_enum_t eVoteOutcome);

std::string GenerateNewAddress(std::string& sError, std::string sName)
{
    LOCK2(cs_main, pwalletMain->cs_wallet);
	{
		if (!pwalletMain->IsLocked(true))
			pwalletMain->TopUpKeyPool();
		// Generate a new key that is added to wallet
		CPubKey newKey;
		if (!pwalletMain->GetKeyFromPool(newKey, false))
		{
			sError = "Keypool ran out, please call keypoolrefill first";
			return std::string();
		}
		CKeyID keyID = newKey.GetID();
		pwalletMain->SetAddressBook(keyID, sName, "receive"); //receive == visible in address book, hidden = non-visible
		LogPrintf(" created new address %s ", CBitcoinAddress(keyID).ToString().c_str());
		return CBitcoinAddress(keyID).ToString();
	}
}

std::string GJE(std::string sKey, std::string sValue, bool bIncludeDelimiter, bool bQuoteValue)
{
	// This is a helper for the Governance gobject create method
	std::string sQ = "\"";
	std::string sOut = sQ + sKey + sQ + ":";
	if (bQuoteValue)
	{
		sOut += sQ + sValue + sQ;
	}
	else
	{
		sOut += sValue;
	}
	if (bIncludeDelimiter) sOut += ",";
	return sOut;
}

std::string RoundToString(double d, int place)
{
	std::ostringstream ss;
    ss << std::fixed << std::setprecision(place) << d ;
    return ss.str() ;
}

double Round(double d, int place)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(place) << d ;
	double r = 0;
	try
	{
		r = boost::lexical_cast<double>(ss.str());
		return r;
	}
	catch(boost::bad_lexical_cast const& e)
	{
		LogPrintf("caught bad lexical cast %f", 1);
		return 0;
	}
	catch(...)
	{
		LogPrintf("caught bad lexical cast %f", 2);
	}
	return r;
}

std::vector<std::string> Split(std::string s, std::string delim)
{
	size_t pos = 0;
	std::string token;
	std::vector<std::string> elems;
	while ((pos = s.find(delim)) != std::string::npos)
	{
		token = s.substr(0, pos);
		elems.push_back(token);
		s.erase(0, pos + delim.length());
	}
	elems.push_back(s);
	return elems;
}

double cdbl(std::string s, int place)
{
	if (s=="") s = "0";
	if (s.length() > 255) return 0;
	s = strReplace(s, "\r","");
	s = strReplace(s, "\n","");
	std::string t = "";
	for (int i = 0; i < (int)s.length(); i++)
	{
		std::string u = s.substr(i,1);
		if (u=="0" || u=="1" || u=="2" || u=="3" || u=="4" || u=="5" || u=="6" || u == "7" || u=="8" || u=="9" || u=="." || u=="-") 
		{
			t += u;
		}
	}
	double r= 0;
	try
	{
	    r = boost::lexical_cast<double>(t);
	}
	catch(boost::bad_lexical_cast const& e)
	{
		LogPrintf("caught cdbl bad lexical cast %f from %s with %f", 1, s, (double)place);
		return 0;
	}
	catch(...)
	{
		LogPrintf("caught cdbl bad lexical cast %f", 2);
	}
	double d = Round(r, place);
	return d;
}

bool Contains(std::string data, std::string instring)
{
	std::size_t found = 0;
	found = data.find(instring);
	if (found != std::string::npos) return true;
	return false;
}

std::string GetElement(std::string sIn, std::string sDelimiter, int iPos)
{
	if (sIn.empty())
		return std::string();
	std::vector<std::string> vInput = Split(sIn.c_str(), sDelimiter);
	if (iPos < (int)vInput.size())
	{
		return vInput[iPos];
	}
	return std::string();
}

std::string GetSporkValue(std::string sKey)
{
	boost::to_upper(sKey);
    std::pair<std::string, int64_t> v = mvApplicationCache[std::make_pair("SPORK", sKey)];
	return v.first;
}

double GetSporkDouble(std::string sName, double nDefault)
{
	double dSetting = cdbl(GetSporkValue(sName), 2);
	if (dSetting == 0) return nDefault;
	return dSetting;
}

std::map<std::string, std::string> GetSporkMap(std::string sPrimaryKey, std::string sSecondaryKey)
{
	boost::to_upper(sPrimaryKey);
	boost::to_upper(sSecondaryKey);
	std::string sDelimiter = "|";
    std::pair<std::string, int64_t> v = mvApplicationCache[std::make_pair(sPrimaryKey, sSecondaryKey)];
	std::vector<std::string> vSporks = Split(v.first, sDelimiter);
	std::map<std::string, std::string> mSporkMap;
	for (int i = 0; i < vSporks.size(); i++)
	{
		std::string sMySpork = vSporks[i];
		if (!sMySpork.empty())
			mSporkMap.insert(std::make_pair(sMySpork, RoundToString(i, 0)));
	}
	return mSporkMap;
}

std::string Left(std::string sSource, int bytes)
{
	if (sSource.length() >= bytes)
	{
		return sSource.substr(0, bytes);
	}
	return std::string();
}	

bool CheckStakeSignature(std::string sBitcoinAddress, std::string sSignature, std::string strMessage, std::string& strError)
{
	CBitcoinAddress addr2(sBitcoinAddress);
	if (!addr2.IsValid()) 
	{
		strError = "Invalid address";
		return false;
	}
	CKeyID keyID2;
	if (!addr2.GetKeyID(keyID2)) 
	{
		strError = "Address does not refer to key";
		return false;
	}
	bool fInvalid = false;
	std::vector<unsigned char> vchSig2 = DecodeBase64(sSignature.c_str(), &fInvalid);
	if (fInvalid)
	{
		strError = "Malformed base64 encoding";
		return false;
	}
	CHashWriter ss2(SER_GETHASH, 0);
	ss2 << strMessageMagic;
	ss2 << strMessage;
	CPubKey pubkey2;
    if (!pubkey2.RecoverCompact(ss2.GetHash(), vchSig2)) 
	{
		strError = "Unable to recover public key.";
		return false;
	}
	bool fSuccess = (pubkey2.GetID() == keyID2);
	return fSuccess;
}


CPK GetCPK(std::string sData)
{
	// CPK DATA FORMAT: sCPK + "|" + Sanitized NickName + "|" + LockTime + "|" + SecurityHash + "|" + CPK Signature + "|" + Email + "|" + VendorType + "|" + OptData
	CPK k;
	std::vector<std::string> vDec = Split(sData.c_str(), "|");
	if (vDec.size() < 5) return k;
	std::string sSecurityHash = vDec[3];
	std::string sSig = vDec[4];
	std::string sCPK = vDec[0];
	if (sCPK.empty()) return k;
	if (vDec.size() >= 6)
		k.sEmail = vDec[5];
	if (vDec.size() >= 7)
		k.sVendorType = vDec[6];
	if (vDec.size() >= 8)
		k.sOptData = vDec[7];

	k.fValid = CheckStakeSignature(sCPK, sSig, sSecurityHash, k.sError);
	if (!k.fValid) 
	{
		LogPrintf("GetCPK::Error Sig %s, SH %s, Err %s, CPK %s, NickName %s ", sSig, sSecurityHash, k.sError, sCPK, vDec[1]);
		return k;
	}

	k.sAddress = sCPK;
	k.sNickName = vDec[1];
	k.nLockTime = (int64_t)cdbl(vDec[2], 0);

	return k;

} 

std::map<std::string, CPK> GetChildMap(std::string sGSCObjType)
{
	std::map<std::string, CPK> mCPKMap;
	boost::to_upper(sGSCObjType);
	int i = 0;
	for (auto ii : mvApplicationCache)
	{
		if (Contains(ii.first.first, sGSCObjType))
		{
			CPK k = GetCPK(ii.second.first);
			i++;
			mCPKMap.insert(std::make_pair(k.sAddress + "-" + RoundToString(i, 0), k));
		}
	}
	return mCPKMap;
}


std::map<std::string, CPK> GetGSCMap(std::string sGSCObjType, std::string sSearch, bool fRequireSig)
{
	std::map<std::string, CPK> mCPKMap;
	boost::to_upper(sGSCObjType);
	for (auto ii : mvApplicationCache)
	{
    	if (ii.first.first == sGSCObjType)
		{
			CPK k = GetCPK(ii.second.first);
			if (!k.sAddress.empty() && k.fValid)
			{
				if ((!sSearch.empty() && (sSearch == k.sAddress || sSearch == k.sNickName)) || sSearch.empty())
				{
					mCPKMap.insert(std::make_pair(k.sAddress, k));
				}
			}
		}
	}
	return mCPKMap;
}

CAmount CAmountFromValue(const UniValue& value)
{
    if (!value.isNum() && !value.isStr()) return 0;
    CAmount amount;
    if (!ParseFixedPoint(value.getValStr(), 8, &amount)) return 0;
    if (!MoneyRange(amount)) return 0;
    return amount;
}

static CCriticalSection csReadWait;
std::string ReadCache(std::string sSection, std::string sKey)
{
	LOCK(csReadWait);
	std::string sLookupSection = sSection;
	std::string sLookupKey = sKey;
	boost::to_upper(sLookupSection);
	boost::to_upper(sLookupKey);
	// NON-CRITICAL TODO : Find a way to eliminate this to_upper while we transition to non-financial transactions
	if (sLookupSection.empty() || sLookupKey.empty())
		return std::string();
	std::pair<std::string, int64_t> t = mvApplicationCache[std::make_pair(sLookupSection, sLookupKey)];
	return t.first;
}

std::string TimestampToHRDate(double dtm)
{
	if (dtm == 0) return "1-1-1970 00:00:00";
	if (dtm > 9888888888) return "1-1-2199 00:00:00";
	std::string sDt = DateTimeStrFormat("%m-%d-%Y %H:%M:%S",dtm);
	return sDt;
}

std::string ExtractXML(std::string XMLdata, std::string key, std::string key_end)
{
	std::string extraction = "";
	std::string::size_type loc = XMLdata.find( key, 0 );
	if( loc != std::string::npos )
	{
		std::string::size_type loc_end = XMLdata.find( key_end, loc+3);
		if (loc_end != std::string::npos )
		{
			extraction = XMLdata.substr(loc+(key.length()),loc_end-loc-(key.length()));
		}
	}
	return extraction;
}


std::string AmountToString(const CAmount& amount)
{
    bool sign = amount < 0;
    int64_t n_abs = (sign ? -amount : amount);
    int64_t quotient = n_abs / COIN;
    int64_t remainder = n_abs % COIN;
	std::string sAmount = strprintf("%s%d.%08d", sign ? "-" : "", quotient, remainder);
	return sAmount;
}

static CBlockIndex* pblockindexFBBHLast;
CBlockIndex* FindBlockByHeight(int nHeight)
{
    CBlockIndex *pblockindex;
	if (nHeight < 0 || nHeight > chainActive.Tip()->nHeight) return NULL;

    if (nHeight < chainActive.Tip()->nHeight / 2)
		pblockindex = mapBlockIndex[chainActive.Genesis()->GetBlockHash()];
    else
        pblockindex = chainActive.Tip();
    if (pblockindexFBBHLast && abs(nHeight - pblockindex->nHeight) > abs(nHeight - pblockindexFBBHLast->nHeight))
        pblockindex = pblockindexFBBHLast;
    while (pblockindex->nHeight > nHeight)
        pblockindex = pblockindex->pprev;
    while (pblockindex->nHeight < nHeight)
		pblockindex = chainActive.Next(pblockindex);
    pblockindexFBBHLast = pblockindex;
    return pblockindex;
}

std::string DefaultRecAddress(std::string sType)
{
	std::string sDefaultRecAddress;
	for (auto item : pwalletMain->mapAddressBook)
    {
        const CBitcoinAddress& address = item.first;
        std::string strName = item.second.name;
        bool fMine = IsMine(*pwalletMain, address.Get());
        if (fMine)
		{
		    sDefaultRecAddress=CBitcoinAddress(address).ToString();
			boost::to_upper(strName);
			boost::to_upper(sType);
			if (strName == sType) 
			{
				sDefaultRecAddress = CBitcoinAddress(address).ToString();
				return sDefaultRecAddress;
			}
		}
    }

	if (!sType.empty())
	{
		std::string sError;
		sDefaultRecAddress = GenerateNewAddress(sError, sType);
		if (sError.empty()) return sDefaultRecAddress;
	}
	
	return sDefaultRecAddress;
}


std::string RetrieveMd5(std::string s1)
{
	try
	{
		const char* chIn = s1.c_str();
		unsigned char digest2[16];
		MD5((unsigned char*)chIn, strlen(chIn), (unsigned char*)&digest2);
		char mdString2[33];
		for(int i = 0; i < 16; i++) sprintf(&mdString2[i*2], "%02x", (unsigned int)digest2[i]);
 		std::string xmd5(mdString2);
		return xmd5;
	}
    catch (std::exception &e)
	{
		return std::string();
	}
}


std::string PubKeyToAddress(const CScript& scriptPubKey)
{
	CTxDestination address1;
    ExtractDestination(scriptPubKey, address1);
    CBitcoinAddress address2(address1);
    return address2.ToString();
}    


void GetTxTimeAndAmountAndHeight(uint256 hashInput, int hashInputOrdinal, int64_t& out_nTime, CAmount& out_caAmount, int& out_height)
{
	CTransaction tx1;
	uint256 hashBlock1;
	if (GetTransaction(hashInput, tx1, Params().GetConsensus(), hashBlock1, true))
	{
		out_caAmount = tx1.vout[hashInputOrdinal].nValue;
		BlockMap::iterator mi = mapBlockIndex.find(hashBlock1);
		if (mi != mapBlockIndex.end())
		{
			CBlockIndex* pindexHistorical = mapBlockIndex[hashBlock1];              
			out_nTime = pindexHistorical->GetBlockTime();
			out_height = pindexHistorical->nHeight;
			return;
		}
		else
		{
			LogPrintf("\nUnable to find hashBlock %s", hashBlock1.GetHex().c_str());
		}
	}
	else
	{
		LogPrintf("\nUnable to find hashblock1 in GetTransaction %s ",hashInput.GetHex().c_str());
	}
}



CAmount GetRPCBalance()
{
	return pwalletMain->GetBalance();
}

bool CreateExternalPurse(std::string& sError)
{
	// This method creates an external purse, used for funding GSC stakes with the wallet locked.
	if (pwalletMain->IsLocked())
	{
		sError = "Wallet must be unlocked.";
		return false;
	}
	std::string sBoinc = DefaultRecAddress("Christian-Public-Key");
	CBitcoinAddress address;
	if (!address.SetString(sBoinc))
	{
	     sError = "Invalid address";
		 return false;
	}
	CKeyID keyID;
	if (!address.GetKeyID(keyID))
	{
		sError = "Address does not refer to a key; Check to ensure wallet is not locked.";
		return false;
	}
	CKey vchSecret;
    if (!pwalletMain->GetKey(keyID, vchSecret)) 
	{
		sError = "Private key for address " + sBoinc + " is not known.  Wallet must be unlocked. ";
		return false;
    }
	std::string ssecret = CBitcoinSecret(vchSecret).ToString();
	// Encrypt the secret, so hackers cannot easily gain access to the privkey - even if they get physical access to the .conf file
	std::string sEncSecret = EncryptAES256(ssecret, sBoinc);
	// Store the pubkey unencrypted and the privkey encrypted in the .conf file:		
	WriteKey("externalpurse", sBoinc);
	ForceSetArg("externalpurse", sBoinc);
	WriteKey("externalprivkey" + sBoinc.substr(0,8), sEncSecret);
	WriteKey("externalpubkey" + sBoinc.substr(0,8), sBoinc);
	ForceSetArg("externalprivkey" + sBoinc.substr(0,8), sEncSecret);
	ForceSetArg("externalpubkey" + sBoinc.substr(0,8), sBoinc);
	std::string sPubFile1 = GetEPArg(true);
	LogPrintf("Rereading pubkey %s \n", sPubFile1);
	return true;	
}

bool FundWithExternalPurse(std::string& sError, const CTxDestination &address, CAmount nValue, bool fSubtractFeeFromAmount, CWalletTx& wtxNew, 
	bool fUseInstantSend, CAmount nExactAmount, std::string sOptionalData, double dMinCoinAge, std::string sPursePubKey)
{

	// ** Note **:  We only allow external purse funded transactions to fund our own destination purse address through GSCs and through coin-age only. 
	// We do not allow purse tx's to fund other addresses or to spend UTXOs for non-coinage based tx's.
	// This is to make it hard to hack an external purse (it would require hacking the encrypted key, then running a fraudulent version of DAC to send the transaction to another recipient.
	// It would also require the hacker to understand how to modify our wallet class to break the safeguards to select the external UTXO's (which is not allowed currently - because our wallet won't sign them for a non-coin-age tx).

    CAmount curBalance = pwalletMain->GetBalance();

    // Check amount
    if (nValue <= 0)
	{
        sError = "Invalid amount";
		return false;
	}
	
	if (nValue > curBalance)
	{
		sError = "Insufficient funds";
		return false;
	}
    // Parse address
    CScript scriptPubKey = GetScriptForDestination(address);

    CReserveKey reservekey(pwalletMain);

    CAmount nFeeRequired;
    std::string strError;
    std::vector<CRecipient> vecSend;
    int nChangePosRet = -1;
	bool fForce = false;
    CRecipient recipient = {scriptPubKey, nValue, fForce, fSubtractFeeFromAmount};
	vecSend.push_back(recipient);
	
    int nMinConfirms = 0;

	// We must pass minCoinAge == .01+, and nExactSpend == purses vout to use this feature:
	
    if (!pwalletMain->CreateTransaction(vecSend, wtxNew, reservekey, nFeeRequired, nChangePosRet, strError, NULL, true, ONLY_DENOMINATED, fUseInstantSend))
	{
        if (!fSubtractFeeFromAmount && nValue + nFeeRequired > pwalletMain->GetBalance())
		{
            sError = strprintf("Error: This transaction requires a transaction fee of at least %s because of its amount, complexity, or use of recently received funds!", FormatMoney(nFeeRequired));
			return false;
		}
		sError = "Unable to Create Transaction: " + strError;
		return false;
    }
    //CValidationState state;
        
    if (!pwalletMain->CommitTransaction(wtxNew, reservekey, g_connman.get(), state,  fUseInstantSend ? NetMsgType::TXLOCKREQUEST : NetMsgType::TX))
	{
        sError = "Error: The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.";
		return false;
	}
	return true;
}


bool RPCSendMoney(std::string& sError, const CTxDestination &address, CAmount nValue, bool fSubtractFeeFromAmount, CWalletTx& wtxNew, bool fUseInstantSend, std::string sOptionalData, double nCoinAge)
{
    CAmount curBalance = pwalletMain->GetBalance();

    // Check amount
    if (nValue <= 0)
	{
        sError = "Invalid amount";
		return false;
	}
	
	if (pwalletMain->IsLocked())
	{
		sError = "Wallet unlock required";
		return false;
	}

	if (nValue > curBalance)
	{
		sError = "Insufficient funds";
		return false;
	}
    // Parse address
    CScript scriptPubKey = GetScriptForDestination(address);

    // Create and send the transaction
    CReserveKey reservekey(pwalletMain);
    CAmount nFeeRequired;
    std::string strError;
    std::vector<CRecipient> vecSend;
    int nChangePosRet = -1;
	bool fForce = false;
    CRecipient recipient = {scriptPubKey, nValue, fForce, fSubtractFeeFromAmount};
	vecSend.push_back(recipient);
	
    int nMinConfirms = 0;
    if (!pwalletMain->CreateTransaction(vecSend, wtxNew, reservekey, nFeeRequired, nChangePosRet, strError, NULL, true,  ONLY_DENOMINATED, fUseInstantSend))
	{
        if (!fSubtractFeeFromAmount && nValue + nFeeRequired > pwalletMain->GetBalance())
		{
            sError = strprintf("Error: This transaction requires a transaction fee of at least %s because of its amount, complexity, or use of recently received funds!", FormatMoney(nFeeRequired));
			return false;
		}
		sError = "Unable to Create Transaction: " + strError;
		return false;
    }
    //CValidationState state;
        
    if (!pwalletMain->CommitTransaction(wtxNew, reservekey, g_connman.get(), state,  fUseInstantSend ? NetMsgType::TXLOCKREQUEST : NetMsgType::TX))
	{
        sError = "Error: The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.";
		return false;
	}
	return true;
}

CAmount R20(CAmount amount)
{
	double nAmount = amount / COIN; 
	nAmount = nAmount + 0.5 - (nAmount < 0); 
	int iAmount = (int)nAmount;
	return (iAmount * COIN);
}

double R2X(double var) 
{ 
    double value = (int)(var * 100 + .5); 
    return (double)value / 100; 
} 

double Quantize(double nFloor, double nCeiling, double nValue)
{
	double nSpan = nCeiling - nFloor;
	double nLevel = nSpan * nValue;
	double nOut = nFloor + nLevel;
	if (nOut > std::max(nFloor, nCeiling)) nOut = std::max(nFloor, nCeiling);
	if (nOut < std::min(nCeiling, nFloor)) nOut = std::min(nCeiling, nFloor);
	return nOut;
}

int GetHeightByEpochTime(int64_t nEpoch)
{
	if (!chainActive.Tip()) return 0;
	int nLast = chainActive.Tip()->nHeight;
	if (nLast < 1) return 0;
	for (int nHeight = nLast; nHeight > 0; nHeight--)
	{
		CBlockIndex* pindex = FindBlockByHeight(nHeight);
		if (pindex)
		{
			int64_t nTime = pindex->GetBlockTime();
			if (nEpoch > nTime) return nHeight;
		}
	}
	return -1;
}

void GetGovSuperblockHeights(int& nNextSuperblock, int& nLastSuperblock)
{
	
    int nBlockHeight = 0;
    {
        LOCK(cs_main);
        nBlockHeight = (int)chainActive.Height();
    }
    int nSuperblockStartBlock = Params().GetConsensus().nSuperblockStartBlock;
    int nSuperblockCycle = Params().GetConsensus().nSuperblockCycle;
    int nFirstSuperblockOffset = (nSuperblockCycle - nSuperblockStartBlock % nSuperblockCycle) % nSuperblockCycle;
    int nFirstSuperblock = nSuperblockStartBlock + nFirstSuperblockOffset;
    if(nBlockHeight < nFirstSuperblock)
	{
        nLastSuperblock = 0;
        nNextSuperblock = nFirstSuperblock;
    } else {
        nLastSuperblock = nBlockHeight - nBlockHeight % nSuperblockCycle;
        nNextSuperblock = nLastSuperblock + nSuperblockCycle;
    }
}

std::string GetActiveProposals()
{
    int nStartTime = GetAdjustedTime() - (86400 * 32);
    LOCK2(cs_main, governance.cs);
    std::vector<CGovernanceObject*> objs = governance.GetAllNewerThan(nStartTime);
	std::string sXML;
	int id = 0;
	std::string sDelim = "|";
	std::string sZero = "\0";
	int nLastSuperblock = 0;
	int nNextSuperblock = 0;
	GetGovSuperblockHeights(nNextSuperblock, nLastSuperblock);
	for (CGovernanceObject* pGovObj : objs)
    {
		if (pGovObj->GetObjectType() != GOVERNANCE_OBJECT_PROPOSAL) continue;
		int64_t nEpoch = 0;
		int64_t nStartEpoch = 0;
		CGovernanceObject* myGov = governance.FindGovernanceObject(pGovObj->GetHash());
		UniValue obj = myGov->GetJSONObject();
		std::string sURL;
		std::string sProposalType;
		nStartEpoch = cdbl(obj["start_epoch"].getValStr(), 0);
		nEpoch = cdbl(obj["end_epoch"].getValStr(), 0);
		sURL = obj["url"].getValStr();
		sProposalType = obj["expensetype"].getValStr();
		if (sProposalType.empty()) sProposalType = "N/A";
		DACProposal dProposal = GetProposalByHash(pGovObj->GetHash(), nLastSuperblock);
		std::string sHash = pGovObj->GetHash().GetHex();
		int nEpochHeight = GetHeightByEpochTime(nStartEpoch);
		// First ensure the proposals gov height has not passed yet
		bool bIsPaid = nEpochHeight < nLastSuperblock;
		std::string sReport = DescribeProposal(dProposal);
		if (fDebug)
			LogPrintf("\nGetActiveProposals::Proposal %s , epochHeight %f, nLastSuperblock %f, IsPaid %f ", 
					sReport, nEpochHeight, nLastSuperblock, (double)bIsPaid);
		if (!bIsPaid)
		{
			int iYes = pGovObj->GetYesCount(VOTE_SIGNAL_FUNDING);
			int iNo = pGovObj->GetNoCount(VOTE_SIGNAL_FUNDING);
			int iAbstain = pGovObj->GetAbstainCount(VOTE_SIGNAL_FUNDING);
			id++;
			if (sProposalType.empty()) sProposalType = "NA";
			std::string sProposalTime = TimestampToHRDate(nStartEpoch);
			if (id == 1) sURL += "&t=" + RoundToString(GetAdjustedTime(), 0);
			std::string sName;
			sName = obj["name"].getValStr();
			double dCharityAmount = 0;
			dCharityAmount = cdbl(obj["payment_amount"].getValStr(), 2);
			std::string sRow = "<proposal>" + sHash + sDelim 
				+ sName + sDelim 
				+ RoundToString(dCharityAmount, 2) + sDelim
				+ sProposalType + sDelim
				+ sProposalTime + sDelim
					+ RoundToString(iYes, 0) + sDelim
					+ RoundToString(iNo, 0) + sDelim + RoundToString(iAbstain,0) 
					+ sDelim + sURL;
				sXML += sRow;
		}
	}
	return sXML;
}


std::string CreateGovernanceCollateral(uint256 GovObjHash, CAmount caFee, std::string& sError)
{
	CWalletTx wtx;
	if(!pwalletMain->GetBudgetSystemCollateralTX(wtx, GovObjHash, caFee, false)) 
	{
		sError = "Error creating collateral transaction for governance object.  Please check your wallet balance and make sure your wallet is unlocked.";
		return std::string();
	}
	if (sError.empty())
	{
		// -- make our change address
		CReserveKey reservekey(pwalletMain);
		//CValidationState state;
        pwalletMain->CommitTransaction(wtx, reservekey, g_connman.get(), state, NetMsgType::TX);
		DBG( cout << "gobject: prepare "
					<< " strData = " << govobj.GetDataAsString()
					<< ", hash = " << govobj.GetHash().GetHex()
					<< ", txidFee = " << wtx.GetHash().GetHex()
					<< endl; );
		return wtx.GetHash().ToString();
	}
	return std::string();
}

int GetNextSuperblock()
{
	int nLastSuperblock, nNextSuperblock;
    // Get current block height
    int nBlockHeight = 0;
    {
        LOCK(cs_main);
        nBlockHeight = (int)chainActive.Height();
    }

    // Get chain parameters
    int nSuperblockStartBlock = Params().GetConsensus().nSuperblockStartBlock;
    int nSuperblockCycle = Params().GetConsensus().nSuperblockCycle;

    // Get first superblock
    int nFirstSuperblockOffset = (nSuperblockCycle - nSuperblockStartBlock % nSuperblockCycle) % nSuperblockCycle;
    int nFirstSuperblock = nSuperblockStartBlock + nFirstSuperblockOffset;

    if(nBlockHeight < nFirstSuperblock)
	{
        nLastSuperblock = 0;
        nNextSuperblock = nFirstSuperblock;
    }
	else 
	{
        nLastSuperblock = nBlockHeight - nBlockHeight % nSuperblockCycle;
        nNextSuperblock = nLastSuperblock + nSuperblockCycle;
    }
	return nNextSuperblock;
}


bool SubmitProposalToNetwork(uint256 txidFee, int64_t nStartTime, std::string sHex, std::string& sError, std::string& out_sGovObj)
{
	if(!masternodeSync.IsBlockchainSynced())
	{
		sError = "Must wait for client to sync with masternode network. ";
		return false;
    }
    // ASSEMBLE NEW GOVERNANCE OBJECT FROM USER PARAMETERS
    uint256 hashParent = uint256();
    int nRevision = 1;
	CGovernanceObject govobj(hashParent, nRevision, nStartTime, txidFee, sHex);
    DBG( cout << "gobject: submit "
             << " strData = " << govobj.GetDataAsString()
             << ", hash = " << govobj.GetHash().GetHex()
             << ", txidFee = " << txidFee.GetHex()
             << endl; );

    std::string strHash = govobj.GetHash().ToString();
    if(!govobj.IsValidLocally(sError, true))
	{
		sError += "Object submission rejected because object is not valid.";
		LogPrintf("\n OBJECT REJECTED:\n gobject submit 0 1 %f %s %s \n", (double)nStartTime, sHex.c_str(), txidFee.GetHex().c_str());
		return false;
    }
    // RELAY THIS OBJECT - Reject if rate check fails but don't update buffer

	bool fRateCheckBypassed = false;
    if(!governance.MasternodeRateCheck(govobj, true, false, fRateCheckBypassed))
	{
        sError = "Object creation rate limit exceeded";
		return false;
	}
    //governance.AddSeenGovernanceObject(govobj.GetHash(), SEEN_OBJECT_IS_VALID);
    govobj.Relay(*g_connman);
    governance.AddGovernanceObject(govobj, *g_connman);
	out_sGovObj = govobj.GetHash().ToString();
	return true;
}

