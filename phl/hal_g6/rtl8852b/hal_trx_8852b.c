/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _HAL_TRX_8852B_C_
#include "rtl8852b_hal.h"

#ifdef PHL_RXSC_AMPDU
static enum rtw_hal_status
_hal_check_rxsc_rdy_8852b(struct hal_info_t *hal,
	struct rtw_phl_com_t *phl_com, u8 *desc, struct rtw_r_meta_data *mdata)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct rtw_rxsc_cache_entry *rxsc_entry = &phl_com->rxsc_entry;

	mdata->tid = GET_RX_AX_DESC_TID_8852B(desc);
	mdata->macid = GET_RX_AX_DESC_MACID_8852B(desc);
	mdata->seq = GET_RX_AX_DESC_SEQ_8852B(desc);
	mdata->mc = GET_RX_AX_DESC_MC_8852B(desc);
	mdata->bc = GET_RX_AX_DESC_BC_8852B(desc);
	mdata->htc = GET_RX_AX_DESC_HTC_8852B(desc);

	if (mdata->ampdu &&
		rxsc_entry->cached_rx_macid == mdata->macid &&
		rxsc_entry->cached_rx_ppdu_cnt == mdata->ppdu_cnt &&
		rxsc_entry->cached_rx_tid == mdata->tid &&
		((rxsc_entry->cached_rx_seq + 1) & WL_SEQ_MASK) == mdata->seq
	) {
		mdata->rx_rate = GET_RX_AX_DESC_RX_DATARATE_8852B(desc);
		mdata->amsdu = GET_RX_AX_DESC_AMSDU_8852B(desc);
		mdata->crc32 = GET_RX_AX_DESC_CRC32_8852B(desc);
		mdata->icverr = GET_RX_AX_DESC_ICVERR_8852B(desc);
		mdata->sw_dec = GET_RX_AX_DESC_SW_DEC_8852B(desc);
		mdata->pwr_bit = GET_RX_AX_DESC_PWR_8852B(desc);
		mdata->q_null = GET_RX_AX_DESC_QNULL_8852B(desc);
		mdata->frame_type = GET_RX_AX_DESC_TYPE_8852B(desc);

		rxsc_entry->cached_rx_seq = mdata->seq;
		hstatus = RTW_HAL_STATUS_SUCCESS;
	} else {
		rxsc_entry->cached_rx_macid = PHL_MACID_MAX_NUM;
	}

	return hstatus;
}
#endif

 /**
 * SW Parsing Rx Desc - hal_parsing_rx_wd_8852b
 * description:
 * 	Parsing Rx WiFi Desc by Halmac or SW Manually
 * input:
 * 	hal : hal ic adapter
 * 	desc : pointer of the start of rx desc
 * output:
 * 	pkt : pointer of the start of pkt;
 * 	pkt_len : the pkt length
 * 	rxwd : rx desc
 */
static enum rtw_hal_status
_hal_parsing_rx_wd_8852b(struct hal_info_t *hal,
	struct rtw_phl_com_t *phl_com,
	u8 *desc, struct rtw_r_meta_data *mdata)
{
	/* ToDo: Parse word by word with byte swap once for
	 * each word
	 */
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_com_t *hal_com = hal->hal_com;
#ifdef PHL_RXSC_AMPDU
	struct rtw_rxsc_cache_entry *rxsc_entry = &phl_com->rxsc_entry;
#endif

	mdata->pktlen = GET_RX_AX_DESC_PKT_LEN_8852B(desc);
	mdata->shift = GET_RX_AX_DESC_SHIFT_8852B(desc);
	mdata->bb_sel = GET_RX_AX_DESC_BB_SEL_8852B(desc);
	mdata->mac_info_vld = GET_RX_AX_DESC_MAC_INFO_VLD_8852B(desc);
	mdata->rpkt_type = GET_RX_AX_DESC_RPKT_TYPE_8852B(desc);
	mdata->drv_info_size = GET_RX_AX_DESC_DRV_INFO_SIZE_8852B(desc);
	mdata->long_rxd = GET_RX_AX_DESC_LONG_RXD_8852B(desc);

	if (   (mdata->rpkt_type == RX_8852B_DESC_PKT_T_WIFI)
	    || (mdata->rpkt_type == RX_8852B_DESC_PKT_T_PPDU_STATUS)) {
		mdata->wl_hd_iv_len = GET_RX_AX_DESC_HDR_IV_L_8852B(desc);

		mdata->ppdu_type = GET_RX_AX_DESC_PPDU_TYPE_8852B(desc);
		mdata->ppdu_cnt = GET_RX_AX_DESC_PPDU_CNT_8852B(desc);
		mdata->ampdu = GET_RX_AX_DESC_AMPDU_8852B(desc);

#ifdef PHL_RXSC_AMPDU
		if (PHL_MACID_MAX_NUM != rxsc_entry->cached_rx_macid &&
			RX_8852B_DESC_PKT_T_WIFI == mdata->rpkt_type) {

			/* check and get rxsc cache */
			if (_hal_check_rxsc_rdy_8852b(hal, phl_com, desc, mdata) == RTW_HAL_STATUS_SUCCESS)
				return RTW_HAL_STATUS_SUCCESS;
		}
#endif

		mdata->sr_en = GET_RX_AX_DESC_SR_EN_8852B(desc);
		mdata->user_id = GET_RX_AX_DESC_USER_ID_8852B(desc);
		mdata->rx_rate = GET_RX_AX_DESC_RX_DATARATE_8852B(desc);
		mdata->rx_gi_ltf = GET_RX_AX_DESC_RX_GI_LTF_8852B(desc);
		mdata->non_srg_ppdu = GET_RX_AX_DESC_NON_SRG_PPDU_8852B(desc);
		mdata->inter_ppdu = GET_RX_AX_DESC_INTER_PPDU_8852B(desc);
		mdata->bw = GET_RX_AX_DESC_BW_8852B(desc);

		mdata->freerun_cnt = GET_RX_AX_DESC_FREERUN_CNT_8852B(desc);

		mdata->a1_match = GET_RX_AX_DESC_A1_MATCH_8852B(desc);
		mdata->sw_dec = GET_RX_AX_DESC_SW_DEC_8852B(desc);
		mdata->hw_dec = GET_RX_AX_DESC_HW_DEC_8852B(desc);
		mdata->ampdu_end_pkt = GET_RX_AX_DESC_AMPDU_EDN_PKT_8852B(desc);
		mdata->amsdu = GET_RX_AX_DESC_AMSDU_8852B(desc);
		mdata->amsdu_cut = GET_RX_AX_DESC_AMSDU_CUT_8852B(desc);
		mdata->last_msdu = GET_RX_AX_DESC_LAST_MSDU_8852B(desc);
		mdata->bypass = GET_RX_AX_DESC_BYPASS_8852B(desc);
		mdata->crc32 = GET_RX_AX_DESC_CRC32_8852B(desc);
		mdata->icverr = GET_RX_AX_DESC_ICVERR_8852B(desc);
		mdata->magic_wake = GET_RX_AX_DESC_MAGIC_WAKE_8852B(desc);
		mdata->unicast_wake = GET_RX_AX_DESC_UNICAST_WAKE_8852B(desc);
		mdata->pattern_wake = GET_RX_AX_DESC_PATTERN_WAKE_8852B(desc);
		mdata->get_ch_info = GET_RX_AX_DESC_CH_INFO_8852B(desc);
		mdata->rx_statistics = GET_RX_AX_DESC_STATISTICS_8852B(desc);

		mdata->pattern_idx = GET_RX_AX_DESC_PATTERN_IDX_8852B(desc);
		mdata->target_idc = GET_RX_AX_DESC_TARGET_IDC_8852B(desc);
		mdata->chksum_ofld_en = GET_RX_AX_DESC_CHKSUM_OFFLOAD_8852B(desc);
		mdata->with_llc = GET_RX_AX_DESC_WITH_LLC_8852B(desc);

	if (mdata->long_rxd == 1) {
			mdata->frame_type = GET_RX_AX_DESC_TYPE_8852B(desc);
			mdata->mc = GET_RX_AX_DESC_MC_8852B(desc);
			mdata->bc = GET_RX_AX_DESC_BC_8852B(desc);
			mdata->more_data = GET_RX_AX_DESC_MD_8852B(desc);
			mdata->more_frag = GET_RX_AX_DESC_MF_8852B(desc);
			mdata->pwr_bit = GET_RX_AX_DESC_PWR_8852B(desc);
			mdata->qos = GET_RX_AX_DESC_QOS_8852B(desc);

			mdata->tid = GET_RX_AX_DESC_TID_8852B(desc);
			mdata->eosp = GET_RX_AX_DESC_EOSP_8852B(desc);
			mdata->htc = GET_RX_AX_DESC_HTC_8852B(desc);
			mdata->q_null = GET_RX_AX_DESC_QNULL_8852B(desc);

			mdata->seq = GET_RX_AX_DESC_SEQ_8852B(desc);
			mdata->frag_num = GET_RX_AX_DESC_FRAG_8852B(desc);

			mdata->sec_cam_idx = GET_RX_AX_DESC_CAM_IDX_8852B(desc);
			mdata->addr_cam = GET_RX_AX_DESC_ADDR_CAM_8852B(desc);

			mdata->addr_cam_vld = GET_RX_AX_DESC_CAM_VLD_8852B(desc);
			if (mdata->addr_cam_vld == 0)
				mdata->macid = 0xFF;
			else
				mdata->macid = GET_RX_AX_DESC_MACID_8852B(desc);

			mdata->rx_pl_id = GET_RX_AX_DESC_PL_ID_8852B(desc);

			mdata->addr_fwd_en = GET_RX_AX_DESC_FWD_EN_8852B(desc);
			mdata->rx_pl_match = GET_RX_AX_DESC_PL_MATCH_8852B(desc);

		_os_mem_cpy(hal_com->drv_priv,
			(void*)&mdata->mac_addr, (void*)(desc + 24), MAC_ALEN);
			mdata->sec_type = GET_RX_AX_DESC_SEC_TYPE_8852B(desc);
		}
	}
#ifdef CONFIG_PHL_CHANNEL_INFO
		else if (mdata->rpkt_type == RX_8852B_DESC_PKT_T_CHANNEL_INFO) {
			mdata->freerun_cnt = GET_RX_AX_DESC_FREERUN_CNT_8852B(desc);
			mdata->bw = GET_RX_AX_DESC_BW_8852B(desc);
			mdata->rx_rate = GET_RX_AX_DESC_RX_DATARATE_8852B(desc);
			/* invalid for 8852B */
			#if 0
			mdata->a1_match = GET_RX_AX_DESC_A1_MATCH_8852B(desc);
			mdata->get_ch_info = GET_RX_AX_DESC_CH_INFO_8852B(desc);
			mdata->macid = GET_RX_AX_DESC_MACID_8852B(desc);
			#endif
		}
#endif

	if(mdata->pktlen == 0)
		hstatus = RTW_HAL_STATUS_FAILURE;
	else
		hstatus = RTW_HAL_STATUS_SUCCESS;

	return hstatus;
}

/**
 * hal_parsing_rx_wd_8852b
 * description:
 * 	Parsing Rx WiFi Desc by Halmac or SW Manually
 * input:
 * 	hal : hal ic adapter
 * 	desc : pointer of the start of rx desc
 * output:
 * 	pkt : pointer of the start of pkt;
 * 	pkt_len : the pkt length
 * 	rxwd : rx desc
 */

static enum rtw_hal_status
hal_parsing_rx_wd_8852b(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal,
				u8 *buf, u8 **pkt, u16 *pkt_len,
				struct rtw_r_meta_data *mdata)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	u8 halmac_rx = 0;
	u8 shift = 0;
	u8 *desc = NULL;
	u8 desc_l = 0;

	do {
		if (NULL == buf)
			break;

		if (phl_com->hci_type == RTW_HCI_PCIE)
			desc = buf + RX_BD_INFO_SIZE;
		else
			desc = buf;

		if (!halmac_rx) {
			hstatus = _hal_parsing_rx_wd_8852b(hal, phl_com, desc, mdata);
		} else {
			/* halmac_ax_ops->parse_rxdesc( */
			/* hm_info->halmac_ax_apter, */
			/* rxpkt_info, */
			/* buf, */
			/* len); */
		}


		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			break;
		/* TODO :: Need Double Check*/
		desc_l = mdata->long_rxd ? RX_DESC_L_SIZE_8852B :
					   RX_DESC_S_SIZE_8852B;

		shift = (u8)(mdata->shift * 2 +
			     mdata->drv_info_size * RX_DESC_DRV_INFO_UNIT_8852B +
			     desc_l);

		if ((1 == mdata->mac_info_vld) &&
		    (RX_8852B_DESC_PKT_T_PPDU_STATUS != mdata->rpkt_type))
			*pkt = desc + shift + RX_PPDU_MAC_INFO_SIZE_8852B;
		else
			*pkt = desc + shift;

		*pkt_len = (u16)mdata->pktlen;

	} while (false);

	return hstatus;
}


static void
_hal_rx_wlanhdr_check_8852b(void *drvpriv, void *hdr, struct rtw_r_meta_data *mdata)
{
	/*Check Retry BIT*/
	u8 retry = 0;
	u16 seq_num = 0;
	u8 type = 0;

	type = PHL_GET_80211_HDR_TYPE(hdr);
	retry = PHL_GET_80211_HDR_RETRY(hdr);
	seq_num = PHL_GET_80211_HDR_SEQUENCE(hdr);
	PHL_GET_80211_HDR_ADDRESS2(drvpriv, hdr, mdata->ta);
	if (retry) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[TP Debug] RETRY PKT!!!! seq_num = %d \n", seq_num);
	}
	if (type == 9) { /* trigger */
		PHL_TRACE(COMP_PHL_TRIG, _PHL_DEBUG_, "rx trigger\n");
		PHL_TRACE(COMP_PHL_TRIG, _PHL_DEBUG_,
			"addr cam hit=%d\n", mdata->addr_cam_vld);
		PHL_TRACE(COMP_PHL_TRIG, _PHL_DEBUG_,
			"addr cam idx=%d\n", mdata->addr_cam);
		PHL_TRACE(COMP_PHL_TRIG, _PHL_DEBUG_,
			"macid = %d\n", mdata->macid);
	}
}

static void
_hal_rx_sts_8852b(struct hal_info_t *hal, struct rtw_r_meta_data *meta)
{
	struct rtw_hal_com_t *hal_com = hal->hal_com;
	struct rtw_trx_stat *trx_stat = &hal_com->trx_stat;

	/* todo: shall drop rx in PHL if hw err */
	if (meta->crc32 || meta->icverr)
		trx_stat->rx_err_cnt++;
	else
		trx_stat->rx_ok_cnt++;
}

enum rtw_hal_status
hal_handle_rx_buffer_8852b(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal,
				u8 *buf, u32 buf_len,
				struct rtw_phl_rx_pkt *phl_rx)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	void *drv = hal_to_drvpriv(hal);
	struct rtw_recv_pkt *r = &phl_rx->r;
	struct rtw_pkt_buf_list *pkt = &r->pkt_list[0];
	struct rtw_r_meta_data *mdata = &r->mdata;
#ifdef PHL_RXSC_AMPDU
	struct rtw_rxsc_cache_entry *rxsc_entry = &phl_com->rxsc_entry;
#endif
#ifdef CONFIG_PHL_TEST_SUITE
	struct test_bp_info bp_info;
#endif
	hstatus = hal_parsing_rx_wd_8852b(phl_com, hal, buf,
					&pkt->vir_addr, &pkt->length, mdata);

	if (RTW_HAL_STATUS_SUCCESS != hstatus)
		return hstatus;
	if( (pkt->vir_addr + pkt->length) > (buf + buf_len) )
		return RTW_HAL_STATUS_FAILURE;

	/* hana_todo */
	r->pkt_cnt = 1;

	switch (mdata->rpkt_type) {
	case RX_8852B_DESC_PKT_T_WIFI :
	{
		phl_rx->type = RTW_RX_TYPE_WIFI;
#ifdef PHL_RXSC_AMPDU /* todo: ppdu status, err ?*/
		if (PHL_MACID_MAX_NUM == rxsc_entry->cached_rx_macid)
#endif
		{
			_hal_rx_wlanhdr_check_8852b(drv, pkt->vir_addr, mdata);
			hal_rx_ppdu_sts_normal_data(phl_com, pkt->vir_addr, mdata);
		}
		_hal_rx_sts_8852b(hal, mdata);
	}
	break;
	case RX_8852B_DESC_PKT_T_TX_PD_RELEASE_HOST :
	{
		phl_rx->type = RTW_RX_TYPE_TX_WP_RELEASE_HOST;
	}
	break;
	case RX_8852B_DESC_PKT_T_PPDU_STATUS :
	{
		struct hal_ppdu_sts ppdu_sts = {0};
		u8 is_su = 1;

		phl_rx->type = RTW_RX_TYPE_PPDU_STATUS;
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "==> RX_8852B_DESC_PKT_T_PPDU_STATUS :band %d ; ppdu_cnt  %d ; freerun_cnt %d \n",
			  mdata->bb_sel, mdata->ppdu_cnt, mdata->freerun_cnt);
		rtw_hal_mac_ax_parse_ppdu_sts(hal, mdata->mac_info_vld,
					      pkt->vir_addr, mdata->pktlen,
					      (void *)&ppdu_sts,
					      (void *)mdata);
		if (ppdu_sts.phy_st_size != 0) {
			if((mdata->ppdu_type == RX_8852B_DESC_PPDU_T_VHT_MU)||
			   (mdata->ppdu_type == RX_8852B_DESC_PPDU_T_HE_MU)||
			   (mdata->ppdu_type == RX_8852B_DESC_PPDU_T_HE_TB)) {
				is_su = 0;
			}
			if(rtw_hal_bb_parse_phy_sts(hal,
						(void *)&ppdu_sts,
						 phl_rx,
						is_su,
						(phl_com->drv_mode == RTW_DRV_MODE_SNIFFER)? true : false
						) != RTW_HAL_STATUS_SUCCESS)
				PHL_TRACE(COMP_PHL_PSTS, _PHL_DEBUG_,
					  "rtw_hal_bb_parse_phy_sts fail\n");



			hal_rx_ppdu_sts(phl_com, phl_rx, &ppdu_sts);
#ifdef CONFIG_PHL_TEST_SUITE
			bp_info.type = BP_INFO_TYPE_MP_RX_PHYSTS;
			bp_info.ptr = (void *)(ppdu_sts.phy_st_ptr);
			bp_info.len = ppdu_sts.phy_st_size;
			rtw_phl_test_setup_bp(phl_com, &bp_info, TEST_SUB_MODULE_MP);
#endif
		}
	}
	break;
	case RX_8852B_DESC_PKT_T_DFS_RPT :
	{
		#ifdef CONFIG_PHL_DFS
		struct mac_ax_dfs_rpt dfs_rpt = {0};
		struct hal_dfs_rpt hal_dfs = {0};
		struct phl_msg msg = {0};
		enum rtw_hal_status mac_status = RTW_HAL_STATUS_FAILURE;

		phl_rx->type = RTW_RX_TYPE_DFS_RPT;

		mac_status = rtw_hal_mac_parse_dfs(hal,pkt->vir_addr, mdata->pktlen, &dfs_rpt);

		if (mac_status == RTW_HAL_STATUS_SUCCESS) {
			#ifdef DBG_PHL_DFS
			PHL_INFO("RX DFS RPT, pkt_len:%d\n", mdata->pktlen);
			PHL_INFO("[DFS] mac-hdr dfs_num:%d\n", dfs_rpt.dfs_num);
			PHL_INFO("[DFS] mac-hdr drop_num:%d\n", dfs_rpt.drop_num);
			PHL_INFO("[DFS] mac-hdr max_cont_drop:%d\n", dfs_rpt.max_cont_drop);
			PHL_INFO("[DFS] mac-hdr total_drop:%d\n", dfs_rpt.total_drop);
			#endif
			hal_dfs.dfs_ptr = dfs_rpt.dfs_ptr;
			hal_dfs.dfs_num = dfs_rpt.dfs_num;
			hal_dfs.phy_idx = 0;

			if (rtw_hal_bb_radar_detect(hal, &hal_dfs)) {
				SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_RX);
				SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_DFS_RD_IS_DETECTING);
				rtw_phl_msg_hub_hal_send(phl_com, NULL, &msg);

				phl_com->dfs_info.is_radar_detectd = true;
				PHL_INFO("[DFS] radar detected\n");
			}
		}
		#endif /*CONFIG_PHL_DFS*/
	}
	break;
	case RX_8852B_DESC_PKT_T_CHANNEL_INFO :
	{
		#ifdef  CONFIG_PHL_CHANNEL_INFO
		enum rtw_hal_status status= RTW_HAL_STATUS_SUCCESS;
		u8* buf_addr;
		struct ch_rpt_hdr_info ch_hdr_rpt = {0};
		struct phy_info_rpt phy_rpt = {0};
		struct ch_info_drv_rpt drv_rpt = {0};
		u32 idle_num = CHAN_INFO_PKT_TOTAL;
		struct chan_info_t *chan_info_old = NULL;

		phl_rx->type = RTW_RX_TYPE_CHANNEL_INFO;
		/* Channel Report */
		/* TODO: need to discuss the final csi header format further.*/
		idle_num = rtw_phl_get_chaninfo_idle_number(drv, phl_com);

		if (idle_num == CHAN_INFO_PKT_TOTAL)
			phl_com->chan_info = rtw_phl_query_idle_chaninfo(drv, phl_com);

		if (phl_com->chan_info == NULL) {
			/*hstatus = RTW_HAL_STATUS_SUCCESS is expected*/
			PHL_INFO("channel info packet not avaialbe due to no pakcet handle\n");
			break;
		}
		buf_addr = phl_com->chan_info->chan_info_buffer;
		status = rtw_hal_bb_ch_info_parsing(hal, pkt->vir_addr, mdata,
			buf_addr + phl_com->chan_info->length,
			&ch_hdr_rpt, &phy_rpt, &drv_rpt);

		if (status == RTW_HAL_STATUS_FAILURE)
			phl_com->chan_info->length = 0;
		else
			phl_com->chan_info->length += drv_rpt.raw_data_len;
		/* store phy info if seg#0 is success*/
		if (drv_rpt.seg_idx_curr == 0 && status != RTW_HAL_STATUS_FAILURE)
			_hal_fill_csi_header_phy_info(hal, &(phl_com->chan_info->csi_header),
				&ch_hdr_rpt, &phy_rpt);
		if (status == RTW_HAL_STATUS_BB_CH_INFO_LAST_SEG) {
			/* Fill remain csi header to buffer  */
			_hal_fill_csi_header_remain(hal,
				&(phl_com->chan_info->csi_header), mdata);
#ifdef CONFIG_PHL_CHANNEL_INFO_DBG
			hal_print_csi_raw_data(phl_com->chan_info);
#endif
			/* push compelete channel info resourecs to busy queue */
			chan_info_old = rtw_phl_recycle_busy_chaninfo(drv, phl_com, phl_com->chan_info);
			if (chan_info_old)
				rtw_phl_enqueue_idle_chaninfo(drv, phl_com, chan_info_old);
			phl_com->chan_info = rtw_phl_query_idle_chaninfo(drv, phl_com);
			if(phl_com->chan_info == NULL)
				PHL_INFO("channel info packet not avaialbe after recycle\n");
		}
		#else
		phl_rx->type = RTW_RX_TYPE_CHANNEL_INFO;
		#endif /* CONFIG_PHL_CHANNEL_INFO */
	}
	break;
	case RX_8852B_DESC_PKT_T_F2P_TX_CMD_RPT :
	{
		/* DL MU Report ; UL OFDMA Trigger Report */
	}
	break;
	case RX_8852B_DESC_PKT_T_C2H :
	{
		struct rtw_c2h_info c = {0};

		phl_rx->type = RTW_RX_TYPE_C2H;
		rtw_hal_mac_parse_c2h(hal, pkt->vir_addr, mdata->pktlen, (void *)&c);

		hal_c2h_post_process(phl_com, hal, (void *)&c);
	}
	break;
	case RX_8852B_DESC_PKT_T_TX_RPT:
	{
		phl_rx->type = RTW_RX_TYPE_TX_RPT;
	}
	break;

	default:
	break;
	}

	return hstatus;
}

