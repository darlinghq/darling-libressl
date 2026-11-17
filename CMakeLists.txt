project(darling-libressl-3-3-6)

# copied and modified from cmake_export_symbol.cmake
function(generate_exp in out)
	file(READ ${in} SYMBOLS)
	string(REGEX REPLACE "\n$" "" SYMBOLS ${SYMBOLS})
	string(REPLACE "\n" "\n_" SYMBOLS ${SYMBOLS})
	string(REGEX REPLACE "(.)$" "\\1\\n" SYMBOLS ${SYMBOLS})
	file(WRITE ${out} "_${SYMBOLS}")
endfunction()

#
# common settings
#
add_compile_definitions(
	LIBRESSL_INTERNAL
	OPENSSL_NO_HW_PADLOCK
	__BEGIN_HIDDEN_DECLS=
	__END_HIDDEN_DECLS=

	HAVE_ASPRINTF
	HAVE_STRCASECMP
	HAVE_STRLCAT
	HAVE_STRLCPY
	HAVE_STRNDUP
	HAVE_STRNLEN
	HAVE_STRSEP
	HAVE_TIMEGM
	HAVE_ARC4RANDOM_BUF
	HAVE_ARC4RANDOM_UNIFORM
	HAVE_GETENTROPY
	HAVE_GETPAGESIZE
	HAVE_TIMINGSAFE_BCMP
	HAVE_MEMMEM
	HAVE_ERR_H
	HAVE_CLOCK_GETTIME
)

add_compile_options(
	-fno-common
	-Wno-pointer-sign
)

include_directories(BEFORE
	include/compat
	include
)

#
# libcompat
#
add_darling_static_library(libressl_3_3_6_libcompat FAT SOURCES
	crypto/compat/freezero.c
	# libmalloc has a private reallocarray only
	crypto/compat/reallocarray.c
	crypto/compat/recallocarray.c
	crypto/compat/explicit_bzero.c
	crypto/compat/timingsafe_memcmp.c
	crypto/compat/syslog_r.c
)

#
# crypto
#

generate_exp(${CMAKE_CURRENT_SOURCE_DIR}/crypto/crypto.sym ${CMAKE_CURRENT_BINARY_DIR}/crypto.exp)

set(DYLIB_INSTALL_NAME "/usr/lib/libcrypto.46.dylib")
set(DYLIB_CURRENT_VERSION "47.2.0")
set(DYLIB_COMPAT_VERSION "47.0.0")

# SMALL_TIME_T needs to be defined on 32-bit and SIZEOF_TIME_T needs to be 4;
# only the files that use these definitions are built per architecture
if (BUILD_TARGET_32BIT)
	add_darling_object_library(crypto46_32 32BIT_ONLY
		crypto/asn1/a_time_tm.c
	)

	target_compile_definitions(crypto46_32 PRIVATE
		SIZEOF_TIME_T=4
		SMALL_TIME_T
	)

	target_include_directories(crypto46_32 PRIVATE
		crypto
	)
endif ()

if (BUILD_TARGET_64BIT)
	add_darling_object_library(crypto46_64 64BIT_ONLY
		crypto/asn1/a_time_tm.c
	)

	target_compile_definitions(crypto46_64 PRIVATE
		SIZEOF_TIME_T=8
	)

	target_include_directories(crypto46_64 PRIVATE
		crypto
	)
endif ()

set(crypto46_source
	crypto/aes/aes_core.c
	crypto/aes/aes_cbc.c
	crypto/camellia/camellia.c
	crypto/camellia/cmll_cbc.c
	crypto/rc4/rc4_enc.c
	crypto/rc4/rc4_skey.c
	crypto/whrlpool/wp_block.c
	crypto/cpt_err.c
	crypto/cryptlib.c
	crypto/crypto_init.c
	crypto/cversion.c
	crypto/ex_data.c
	crypto/malloc-wrapper.c
	crypto/mem_clr.c
	crypto/mem_dbg.c
	crypto/o_init.c
	crypto/o_str.c
	crypto/o_time.c
	crypto/aes/aes_cfb.c
	crypto/aes/aes_ctr.c
	crypto/aes/aes_ecb.c
	crypto/aes/aes_ige.c
	crypto/aes/aes_misc.c
	crypto/aes/aes_ofb.c
	crypto/aes/aes_wrap.c
	crypto/asn1/a_bitstr.c
	crypto/asn1/a_bool.c
	crypto/asn1/a_d2i_fp.c
	crypto/asn1/a_digest.c
	crypto/asn1/a_dup.c
	crypto/asn1/a_enum.c
	crypto/asn1/a_i2d_fp.c
	crypto/asn1/a_int.c
	crypto/asn1/a_mbstr.c
	crypto/asn1/a_object.c
	crypto/asn1/a_octet.c
	crypto/asn1/a_print.c
	crypto/asn1/a_sign.c
	crypto/asn1/a_strex.c
	crypto/asn1/a_strnid.c
	crypto/asn1/a_time.c
	crypto/asn1/a_type.c
	crypto/asn1/a_utf8.c
	crypto/asn1/a_verify.c
	crypto/asn1/ameth_lib.c
	crypto/asn1/asn1_err.c
	crypto/asn1/asn1_gen.c
	crypto/asn1/asn1_lib.c
	crypto/asn1/asn1_par.c
	crypto/asn1/asn_mime.c
	crypto/asn1/asn_moid.c
	crypto/asn1/asn_pack.c
	crypto/asn1/bio_asn1.c
	crypto/asn1/bio_ndef.c
	crypto/asn1/d2i_pr.c
	crypto/asn1/d2i_pu.c
	crypto/asn1/evp_asn1.c
	crypto/asn1/f_enum.c
	crypto/asn1/f_int.c
	crypto/asn1/f_string.c
	crypto/asn1/i2d_pr.c
	crypto/asn1/i2d_pu.c
	crypto/asn1/n_pkey.c
	crypto/asn1/nsseq.c
	crypto/asn1/p5_pbe.c
	crypto/asn1/p5_pbev2.c
	crypto/asn1/p8_pkey.c
	crypto/asn1/t_bitst.c
	crypto/asn1/t_crl.c
	crypto/asn1/t_pkey.c
	crypto/asn1/t_req.c
	crypto/asn1/t_spki.c
	crypto/asn1/t_x509.c
	crypto/asn1/t_x509a.c
	crypto/asn1/tasn_dec.c
	crypto/asn1/tasn_enc.c
	crypto/asn1/tasn_fre.c
	crypto/asn1/tasn_new.c
	crypto/asn1/tasn_prn.c
	crypto/asn1/tasn_typ.c
	crypto/asn1/tasn_utl.c
	crypto/asn1/x_algor.c
	crypto/asn1/x_attrib.c
	crypto/asn1/x_bignum.c
	crypto/asn1/x_crl.c
	crypto/asn1/x_exten.c
	crypto/asn1/x_info.c
	crypto/asn1/x_long.c
	crypto/asn1/x_name.c
	crypto/asn1/x_nx509.c
	crypto/asn1/x_pkey.c
	crypto/asn1/x_pubkey.c
	crypto/asn1/x_req.c
	crypto/asn1/x_sig.c
	crypto/asn1/x_spki.c
	crypto/asn1/x_val.c
	crypto/asn1/x_x509.c
	crypto/asn1/x_x509a.c
	crypto/bf/bf_cfb64.c
	crypto/bf/bf_ecb.c
	crypto/bf/bf_enc.c
	crypto/bf/bf_ofb64.c
	crypto/bf/bf_skey.c
	crypto/bio/b_dump.c
	crypto/bio/b_print.c
	crypto/bio/b_sock.c
	crypto/bio/bf_buff.c
	crypto/bio/bf_nbio.c
	crypto/bio/bf_null.c
	crypto/bio/bio_cb.c
	crypto/bio/bio_err.c
	crypto/bio/bio_lib.c
	crypto/bio/bio_meth.c
	crypto/bio/bss_acpt.c
	crypto/bio/bss_bio.c
	crypto/bio/bss_conn.c
	crypto/bio/bss_dgram.c
	crypto/bio/bss_fd.c
	crypto/bio/bss_file.c
	crypto/bio/bss_mem.c
	crypto/bio/bss_null.c
	crypto/bio/bss_sock.c
	crypto/bn/bn_add.c
	crypto/bn/bn_asm.c
	crypto/bn/bn_blind.c
	crypto/bn/bn_const.c
	crypto/bn/bn_ctx.c
	crypto/bn/bn_depr.c
	crypto/bn/bn_div.c
	crypto/bn/bn_err.c
	crypto/bn/bn_exp.c
	crypto/bn/bn_exp2.c
	crypto/bn/bn_gcd.c
	crypto/bn/bn_gf2m.c
	crypto/bn/bn_kron.c
	crypto/bn/bn_lib.c
	crypto/bn/bn_mod.c
	crypto/bn/bn_mont.c
	crypto/bn/bn_mpi.c
	crypto/bn/bn_mul.c
	crypto/bn/bn_nist.c
	crypto/bn/bn_prime.c
	crypto/bn/bn_print.c
	crypto/bn/bn_rand.c
	crypto/bn/bn_recp.c
	crypto/bn/bn_shift.c
	crypto/bn/bn_sqr.c
	crypto/bn/bn_sqrt.c
	crypto/bn/bn_word.c
	crypto/bn/bn_x931p.c
	crypto/buffer/buf_err.c
	crypto/buffer/buf_str.c
	crypto/buffer/buffer.c
	crypto/camellia/cmll_cfb.c
	crypto/camellia/cmll_ctr.c
	crypto/camellia/cmll_ecb.c
	crypto/camellia/cmll_misc.c
	crypto/camellia/cmll_ofb.c
	crypto/cast/c_cfb64.c
	crypto/cast/c_ecb.c
	crypto/cast/c_enc.c
	crypto/cast/c_ofb64.c
	crypto/cast/c_skey.c
	crypto/chacha/chacha.c
	crypto/cmac/cm_ameth.c
	crypto/cmac/cm_pmeth.c
	crypto/cmac/cmac.c
	crypto/cms/cms_asn1.c
	crypto/cms/cms_att.c
	crypto/cms/cms_cd.c
	crypto/cms/cms_dd.c
	crypto/cms/cms_enc.c
	crypto/cms/cms_env.c
	crypto/cms/cms_err.c
	crypto/cms/cms_ess.c
	crypto/cms/cms_io.c
	crypto/cms/cms_kari.c
	crypto/cms/cms_lib.c
	crypto/cms/cms_pwri.c
	crypto/cms/cms_sd.c
	crypto/cms/cms_smime.c
	crypto/comp/c_rle.c
	crypto/comp/c_zlib.c
	crypto/comp/comp_err.c
	crypto/comp/comp_lib.c
	crypto/conf/conf_api.c
	crypto/conf/conf_def.c
	crypto/conf/conf_err.c
	crypto/conf/conf_lib.c
	crypto/conf/conf_mall.c
	crypto/conf/conf_mod.c
	crypto/conf/conf_sap.c
	crypto/curve25519/curve25519-generic.c
	crypto/curve25519/curve25519.c
	crypto/des/cbc_cksm.c
	crypto/des/cbc_enc.c
	crypto/des/cfb64ede.c
	crypto/des/cfb64enc.c
	crypto/des/cfb_enc.c
	crypto/des/des_enc.c
	crypto/des/ecb3_enc.c
	crypto/des/ecb_enc.c
	crypto/des/ede_cbcm_enc.c
	crypto/des/enc_read.c
	crypto/des/enc_writ.c
	crypto/des/fcrypt.c
	crypto/des/fcrypt_b.c
	crypto/des/ofb64ede.c
	crypto/des/ofb64enc.c
	crypto/des/ofb_enc.c
	crypto/des/pcbc_enc.c
	crypto/des/qud_cksm.c
	crypto/des/rand_key.c
	crypto/des/set_key.c
	crypto/des/str2key.c
	crypto/des/xcbc_enc.c
	crypto/dh/dh_ameth.c
	crypto/dh/dh_asn1.c
	crypto/dh/dh_check.c
	crypto/dh/dh_depr.c
	crypto/dh/dh_err.c
	crypto/dh/dh_gen.c
	crypto/dh/dh_key.c
	crypto/dh/dh_lib.c
	crypto/dh/dh_pmeth.c
	crypto/dh/dh_prn.c
	crypto/dsa/dsa_ameth.c
	crypto/dsa/dsa_asn1.c
	crypto/dsa/dsa_depr.c
	crypto/dsa/dsa_err.c
	crypto/dsa/dsa_gen.c
	crypto/dsa/dsa_key.c
	crypto/dsa/dsa_lib.c
	crypto/dsa/dsa_meth.c
	crypto/dsa/dsa_ossl.c
	crypto/dsa/dsa_pmeth.c
	crypto/dsa/dsa_prn.c
	crypto/dsa/dsa_sign.c
	crypto/dsa/dsa_vrf.c
	crypto/dso/dso_dlfcn.c
	crypto/dso/dso_err.c
	crypto/dso/dso_lib.c
	crypto/dso/dso_null.c
	crypto/dso/dso_openssl.c
	crypto/ec/ec2_mult.c
	crypto/ec/ec2_oct.c
	crypto/ec/ec2_smpl.c
	crypto/ec/ec_ameth.c
	crypto/ec/ec_asn1.c
	crypto/ec/ec_check.c
	crypto/ec/ec_curve.c
	crypto/ec/ec_cvt.c
	crypto/ec/ec_err.c
	crypto/ec/ec_key.c
	crypto/ec/ec_kmeth.c
	crypto/ec/ec_lib.c
	crypto/ec/ec_mult.c
	crypto/ec/ec_oct.c
	crypto/ec/ec_pmeth.c
	crypto/ec/ec_print.c
	crypto/ec/eck_prn.c
	crypto/ec/ecp_mont.c
	crypto/ec/ecp_nist.c
	crypto/ec/ecp_oct.c
	crypto/ec/ecp_smpl.c
	crypto/ecdh/ecdh_kdf.c
	crypto/ecdh/ech_err.c
	crypto/ecdh/ech_key.c
	crypto/ecdh/ech_lib.c
	crypto/ecdsa/ecs_asn1.c
	crypto/ecdsa/ecs_err.c
	crypto/ecdsa/ecs_lib.c
	crypto/ecdsa/ecs_ossl.c
	crypto/ecdsa/ecs_sign.c
	crypto/ecdsa/ecs_vrf.c
	crypto/engine/eng_all.c
	crypto/engine/eng_cnf.c
	crypto/engine/eng_ctrl.c
	crypto/engine/eng_dyn.c
	crypto/engine/eng_err.c
	crypto/engine/eng_fat.c
	crypto/engine/eng_init.c
	crypto/engine/eng_lib.c
	crypto/engine/eng_list.c
	crypto/engine/eng_openssl.c
	crypto/engine/eng_pkey.c
	crypto/engine/eng_table.c
	crypto/engine/tb_asnmth.c
	crypto/engine/tb_cipher.c
	crypto/engine/tb_dh.c
	crypto/engine/tb_digest.c
	crypto/engine/tb_dsa.c
	crypto/engine/tb_ecdh.c
	crypto/engine/tb_ecdsa.c
	crypto/engine/tb_eckey.c
	crypto/engine/tb_pkmeth.c
	crypto/engine/tb_rand.c
	crypto/engine/tb_rsa.c
	crypto/engine/tb_store.c
	crypto/err/err.c
	crypto/err/err_all.c
	crypto/err/err_prn.c
	crypto/evp/bio_b64.c
	crypto/evp/bio_enc.c
	crypto/evp/bio_md.c
	crypto/evp/c_all.c
	crypto/evp/digest.c
	crypto/evp/e_aes.c
	crypto/evp/e_aes_cbc_hmac_sha1.c
	crypto/evp/e_bf.c
	crypto/evp/e_camellia.c
	crypto/evp/e_cast.c
	crypto/evp/e_chacha.c
	crypto/evp/e_chacha20poly1305.c
	crypto/evp/e_des.c
	crypto/evp/e_des3.c
	crypto/evp/e_gost2814789.c
	crypto/evp/e_idea.c
	crypto/evp/e_null.c
	crypto/evp/e_old.c
	crypto/evp/e_rc2.c
	crypto/evp/e_rc4.c
	crypto/evp/e_rc4_hmac_md5.c
	crypto/evp/e_sm4.c
	crypto/evp/e_xcbc_d.c
	crypto/evp/encode.c
	crypto/evp/evp_aead.c
	crypto/evp/evp_enc.c
	crypto/evp/evp_err.c
	crypto/evp/evp_key.c
	crypto/evp/evp_lib.c
	crypto/evp/evp_pbe.c
	crypto/evp/evp_pkey.c
	crypto/evp/m_dss.c
	crypto/evp/m_dss1.c
	crypto/evp/m_ecdsa.c
	crypto/evp/m_gost2814789.c
	crypto/evp/m_gostr341194.c
	crypto/evp/m_md4.c
	crypto/evp/m_md5.c
	crypto/evp/m_md5_sha1.c
	crypto/evp/m_null.c
	crypto/evp/m_ripemd.c
	crypto/evp/m_sha1.c
	crypto/evp/m_sigver.c
	crypto/evp/m_streebog.c
	crypto/evp/m_sm3.c
	crypto/evp/m_wp.c
	crypto/evp/names.c
	crypto/evp/p5_crpt.c
	crypto/evp/p5_crpt2.c
	crypto/evp/p_dec.c
	crypto/evp/p_enc.c
	crypto/evp/p_lib.c
	crypto/evp/p_open.c
	crypto/evp/p_seal.c
	crypto/evp/p_sign.c
	crypto/evp/p_verify.c
	crypto/evp/pmeth_fn.c
	crypto/evp/pmeth_gn.c
	crypto/evp/pmeth_lib.c
	crypto/gost/gost2814789.c
	crypto/gost/gost89_keywrap.c
	crypto/gost/gost89_params.c
	crypto/gost/gost89imit_ameth.c
	crypto/gost/gost89imit_pmeth.c
	crypto/gost/gost_asn1.c
	crypto/gost/gost_err.c
	crypto/gost/gostr341001.c
	crypto/gost/gostr341001_ameth.c
	crypto/gost/gostr341001_key.c
	crypto/gost/gostr341001_params.c
	crypto/gost/gostr341001_pmeth.c
	crypto/gost/gostr341194.c
	crypto/gost/streebog.c
	crypto/hkdf/hkdf.c
	crypto/hmac/hm_ameth.c
	crypto/hmac/hm_pmeth.c
	crypto/hmac/hmac.c
	crypto/idea/i_cbc.c
	crypto/idea/i_cfb64.c
	crypto/idea/i_ecb.c
	crypto/idea/i_ofb64.c
	crypto/idea/i_skey.c
	crypto/lhash/lh_stats.c
	crypto/lhash/lhash.c
	crypto/md4/md4_dgst.c
	crypto/md4/md4_one.c
	crypto/md5/md5_dgst.c
	crypto/md5/md5_one.c
	crypto/modes/cbc128.c
	crypto/modes/ccm128.c
	crypto/modes/cfb128.c
	crypto/modes/ctr128.c
	crypto/modes/cts128.c
	crypto/modes/gcm128.c
	crypto/modes/ofb128.c
	crypto/modes/xts128.c
	crypto/objects/o_names.c
	crypto/objects/obj_dat.c
	crypto/objects/obj_err.c
	crypto/objects/obj_lib.c
	crypto/objects/obj_xref.c
	crypto/ocsp/ocsp_asn.c
	crypto/ocsp/ocsp_cl.c
	crypto/ocsp/ocsp_err.c
	crypto/ocsp/ocsp_ext.c
	crypto/ocsp/ocsp_ht.c
	crypto/ocsp/ocsp_lib.c
	crypto/ocsp/ocsp_prn.c
	crypto/ocsp/ocsp_srv.c
	crypto/ocsp/ocsp_vfy.c
	crypto/pem/pem_all.c
	crypto/pem/pem_err.c
	crypto/pem/pem_info.c
	crypto/pem/pem_lib.c
	crypto/pem/pem_oth.c
	crypto/pem/pem_pk8.c
	crypto/pem/pem_pkey.c
	crypto/pem/pem_seal.c
	crypto/pem/pem_sign.c
	crypto/pem/pem_x509.c
	crypto/pem/pem_xaux.c
	crypto/pem/pvkfmt.c
	crypto/pkcs12/p12_add.c
	crypto/pkcs12/p12_asn.c
	crypto/pkcs12/p12_attr.c
	crypto/pkcs12/p12_crpt.c
	crypto/pkcs12/p12_crt.c
	crypto/pkcs12/p12_decr.c
	crypto/pkcs12/p12_init.c
	crypto/pkcs12/p12_key.c
	crypto/pkcs12/p12_kiss.c
	crypto/pkcs12/p12_mutl.c
	crypto/pkcs12/p12_npas.c
	crypto/pkcs12/p12_p8d.c
	crypto/pkcs12/p12_p8e.c
	crypto/pkcs12/p12_utl.c
	crypto/pkcs12/pk12err.c
	crypto/pkcs7/bio_pk7.c
	crypto/pkcs7/pk7_asn1.c
	crypto/pkcs7/pk7_attr.c
	crypto/pkcs7/pk7_doit.c
	crypto/pkcs7/pk7_lib.c
	crypto/pkcs7/pk7_mime.c
	crypto/pkcs7/pk7_smime.c
	crypto/pkcs7/pkcs7err.c
	crypto/poly1305/poly1305.c
	crypto/rand/rand_err.c
	crypto/rand/rand_lib.c
	crypto/rand/randfile.c
	crypto/rc2/rc2_cbc.c
	crypto/rc2/rc2_ecb.c
	crypto/rc2/rc2_skey.c
	crypto/rc2/rc2cfb64.c
	crypto/rc2/rc2ofb64.c
	crypto/ripemd/rmd_dgst.c
	crypto/ripemd/rmd_one.c
	crypto/rsa/rsa_ameth.c
	crypto/rsa/rsa_asn1.c
	crypto/rsa/rsa_chk.c
	crypto/rsa/rsa_crpt.c
	crypto/rsa/rsa_depr.c
	crypto/rsa/rsa_eay.c
	crypto/rsa/rsa_err.c
	crypto/rsa/rsa_gen.c
	crypto/rsa/rsa_lib.c
	crypto/rsa/rsa_meth.c
	crypto/rsa/rsa_none.c
	crypto/rsa/rsa_oaep.c
	crypto/rsa/rsa_pk1.c
	crypto/rsa/rsa_pmeth.c
	crypto/rsa/rsa_prn.c
	crypto/rsa/rsa_pss.c
	crypto/rsa/rsa_saos.c
	crypto/rsa/rsa_sign.c
	crypto/rsa/rsa_x931.c
	crypto/sha/sha1_one.c
	crypto/sha/sha1dgst.c
	crypto/sha/sha256.c
	crypto/sha/sha512.c
	crypto/sm3/sm3.c
	crypto/sm4/sm4.c
	crypto/stack/stack.c
	crypto/ts/ts_asn1.c
	crypto/ts/ts_conf.c
	crypto/ts/ts_err.c
	crypto/ts/ts_lib.c
	crypto/ts/ts_req_print.c
	crypto/ts/ts_req_utils.c
	crypto/ts/ts_rsp_print.c
	crypto/ts/ts_rsp_sign.c
	crypto/ts/ts_rsp_utils.c
	crypto/ts/ts_rsp_verify.c
	crypto/ts/ts_verify_ctx.c
	crypto/txt_db/txt_db.c
	crypto/ui/ui_err.c
	crypto/ui/ui_lib.c
	crypto/ui/ui_util.c
	crypto/whrlpool/wp_dgst.c
	crypto/x509/by_dir.c
	crypto/x509/by_file.c
	crypto/x509/by_mem.c
	crypto/x509/pcy_cache.c
	crypto/x509/pcy_data.c
	crypto/x509/pcy_lib.c
	crypto/x509/pcy_map.c
	crypto/x509/pcy_node.c
	crypto/x509/pcy_tree.c
	crypto/x509/x509_akey.c
	crypto/x509/x509_akeya.c
	crypto/x509/x509_alt.c
	crypto/x509/x509_att.c
	crypto/x509/x509_bcons.c
	crypto/x509/x509_bitst.c
	crypto/x509/x509_cmp.c
	crypto/x509/x509_conf.c
	crypto/x509/x509_constraints.c
	crypto/x509/x509_cpols.c
	crypto/x509/x509_crld.c
	crypto/x509/x509_d2.c
	crypto/x509/x509_def.c
	crypto/x509/x509_enum.c
	crypto/x509/x509_err.c
	crypto/x509/x509_ext.c
	crypto/x509/x509_extku.c
	crypto/x509/x509_genn.c
	crypto/x509/x509_ia5.c
	crypto/x509/x509_info.c
	crypto/x509/x509_int.c
	crypto/x509/x509_issuer_cache.c
	crypto/x509/x509_lib.c
	crypto/x509/x509_lu.c
	crypto/x509/x509_ncons.c
	crypto/x509/x509_obj.c
	crypto/x509/x509_ocsp.c
	crypto/x509/x509_pci.c
	crypto/x509/x509_pcia.c
	crypto/x509/x509_pcons.c
	crypto/x509/x509_pku.c
	crypto/x509/x509_pmaps.c
	crypto/x509/x509_prn.c
	crypto/x509/x509_purp.c
	crypto/x509/x509_r2x.c
	crypto/x509/x509_req.c
	crypto/x509/x509_set.c
	crypto/x509/x509_skey.c
	crypto/x509/x509_sxnet.c
	crypto/x509/x509_trs.c
	crypto/x509/x509_txt.c
	crypto/x509/x509_utl.c
	crypto/x509/x509_v3.c
	crypto/x509/x509_verify.c
	crypto/x509/x509_vfy.c
	crypto/x509/x509_vpm.c
	crypto/x509/x509cset.c
	crypto/x509/x509name.c
	crypto/x509/x509rset.c
	crypto/x509/x509spki.c
	crypto/x509/x509type.c
	crypto/x509/x_all.c
	crypto/crypto_lock.c
	crypto/bio/b_posix.c
	crypto/bio/bss_log.c
	crypto/ui/ui_openssl.c
)

if (BUILD_TARGET_32BIT)
	set (crypto46_source ${crypto46_source}
		$<TARGET_OBJECTS:crypto46_32>
	)
endif ()

if (BUILD_TARGET_64BIT)
	set (crypto46_source ${crypto46_source}
		$<TARGET_OBJECTS:crypto46_64>
	)
endif ()

add_darling_library(crypto46
	${crypto46_source}
)

set_source_files_properties(crypto/compat/explicit_bzero.c PROPERTIES
	COMPILE_FLAGS -O0
)

target_compile_definitions(crypto46 PRIVATE
	OPENSSL_NO_ASM
	OPENSSLDIR="/etc/ssl"
)

# bn_nist.c accesses BN_ULONG limbs through unsigned int pointers; with strict
# aliasing the optimized 64-bit P-256/P-384 reduction returns wrong residues
target_compile_options(crypto46 PRIVATE
	-fno-strict-aliasing
)

target_include_directories(crypto46 PRIVATE
	crypto
	crypto/asn1
	crypto/bn
	crypto/dsa
	crypto/ec
	crypto/ecdh
	crypto/ecdsa
	crypto/evp
	crypto/modes
)

target_link_libraries(crypto46 PRIVATE
	libressl_3_3_6_libcompat
	system
)

set_target_properties(crypto46 PROPERTIES
	OUTPUT_NAME "crypto.46"
)

target_link_options(crypto46 PRIVATE
	-Wl,-exported_symbols_list,${CMAKE_CURRENT_BINARY_DIR}/crypto.exp
)

#
# ssl
#

generate_exp(${CMAKE_CURRENT_SOURCE_DIR}/ssl/ssl.sym ${CMAKE_CURRENT_BINARY_DIR}/ssl.exp)

set(DYLIB_INSTALL_NAME "/usr/lib/libssl.48.dylib")
set(DYLIB_CURRENT_VERSION "49.2.0")
set(DYLIB_COMPAT_VERSION "49.0.0")

add_darling_library(ssl48
	ssl/bio_ssl.c
	ssl/bs_ber.c
	ssl/bs_cbb.c
	ssl/bs_cbs.c
	ssl/d1_both.c
	ssl/d1_lib.c
	ssl/d1_pkt.c
	ssl/d1_srtp.c
	ssl/pqueue.c
	ssl/s3_cbc.c
	ssl/s3_lib.c
	ssl/ssl_algs.c
	ssl/ssl_asn1.c
	ssl/ssl_both.c
	ssl/ssl_cert.c
	ssl/ssl_ciph.c
	ssl/ssl_ciphers.c
	ssl/ssl_clnt.c
	ssl/ssl_err.c
	ssl/ssl_init.c
	ssl/ssl_kex.c
	ssl/ssl_lib.c
	ssl/ssl_methods.c
	ssl/ssl_packet.c
	ssl/ssl_pkt.c
	ssl/ssl_rsa.c
	ssl/ssl_sess.c
	ssl/ssl_sigalgs.c
	ssl/ssl_srvr.c
	ssl/ssl_stat.c
	ssl/ssl_tlsext.c
	ssl/ssl_transcript.c
	ssl/ssl_txt.c
	ssl/ssl_versions.c
	ssl/t1_enc.c
	ssl/t1_lib.c
	ssl/tls12_record_layer.c
	ssl/tls13_buffer.c
	ssl/tls13_client.c
	ssl/tls13_error.c
	ssl/tls13_handshake.c
	ssl/tls13_handshake_msg.c
	ssl/tls13_key_schedule.c
	ssl/tls13_key_share.c
	ssl/tls13_legacy.c
	ssl/tls13_lib.c
	ssl/tls13_record.c
	ssl/tls13_record_layer.c
	ssl/tls13_server.c
)

target_include_directories(ssl48 PRIVATE
	ssl
)

target_link_libraries(ssl48 PRIVATE
	libressl_3_3_6_libcompat
	crypto46
	system
)

set_target_properties(ssl48 PROPERTIES
	OUTPUT_NAME "ssl.48"
)

target_link_options(ssl48 PRIVATE
	-Wl,-exported_symbols_list,${CMAKE_CURRENT_BINARY_DIR}/ssl.exp
)

#
# tls
#

generate_exp(${CMAKE_CURRENT_SOURCE_DIR}/tls/tls.sym ${CMAKE_CURRENT_BINARY_DIR}/tls.exp)

set(DYLIB_INSTALL_NAME "/usr/lib/libtls.20.dylib")
set(DYLIB_CURRENT_VERSION "21.3.0")
set(DYLIB_COMPAT_VERSION "21.0.0")

add_darling_library(tls20
	tls/tls.c
	tls/tls_bio_cb.c
	tls/tls_client.c
	tls/tls_config.c
	tls/tls_conninfo.c
	tls/tls_keypair.c
	tls/tls_server.c
	tls/tls_ocsp.c
	tls/tls_peer.c
	tls/tls_util.c
	tls/tls_verify.c
	tls/compat/ftruncate.c
	tls/compat/pread.c
	tls/compat/pwrite.c
)

target_compile_definitions(tls20 PRIVATE
	TLS_DEFAULT_CA_FILE="/etc/ssl/cert.pem"
)

target_include_directories(tls20 PRIVATE
	tls
)

target_link_libraries(tls20 PRIVATE
	libressl_3_3_6_libcompat
	ssl48
	crypto46
	system
)

set_target_properties(tls20 PROPERTIES
	OUTPUT_NAME "tls.20"
)

target_link_options(tls20 PRIVATE
	-Wl,-exported_symbols_list,${CMAKE_CURRENT_BINARY_DIR}/tls.exp
)

#
# `openssl` command
#

add_darling_executable(openssl_3_3_6
	apps/openssl/apps.c
	apps/openssl/asn1pars.c
	apps/openssl/ca.c
	apps/openssl/ciphers.c
	apps/openssl/crl.c
	apps/openssl/crl2p7.c
	apps/openssl/cms.c
	apps/openssl/dgst.c
	apps/openssl/dh.c
	apps/openssl/dhparam.c
	apps/openssl/dsa.c
	apps/openssl/dsaparam.c
	apps/openssl/ec.c
	apps/openssl/ecparam.c
	apps/openssl/enc.c
	apps/openssl/errstr.c
	apps/openssl/gendh.c
	apps/openssl/gendsa.c
	apps/openssl/genpkey.c
	apps/openssl/genrsa.c
	apps/openssl/nseq.c
	apps/openssl/ocsp.c
	apps/openssl/openssl.c
	apps/openssl/passwd.c
	apps/openssl/pkcs12.c
	apps/openssl/pkcs7.c
	apps/openssl/pkcs8.c
	apps/openssl/pkey.c
	apps/openssl/pkeyparam.c
	apps/openssl/pkeyutl.c
	apps/openssl/prime.c
	apps/openssl/rand.c
	apps/openssl/req.c
	apps/openssl/rsa.c
	apps/openssl/rsautl.c
	apps/openssl/s_cb.c
	apps/openssl/s_client.c
	apps/openssl/s_server.c
	apps/openssl/s_socket.c
	apps/openssl/s_time.c
	apps/openssl/sess_id.c
	apps/openssl/smime.c
	apps/openssl/speed.c
	apps/openssl/spkac.c
	apps/openssl/ts.c
	apps/openssl/verify.c
	apps/openssl/version.c
	apps/openssl/x509.c
	apps/openssl/apps_posix.c
	apps/openssl/certhash.c
	apps/openssl/compat/strtonum.c
)

set_target_properties(openssl_3_3_6 PROPERTIES
	OUTPUT_NAME "openssl"
)

target_include_directories(openssl_3_3_6 PRIVATE
	apps/openssl
)

target_link_libraries(openssl_3_3_6
	libressl_3_3_6_libcompat
	crypto46
	ssl48
)

# install everything
install(TARGETS crypto46 ssl48 tls20 DESTINATION libexec/darling/usr/lib)
install(TARGETS openssl_3_3_6 DESTINATION libexec/darling/usr/bin)

install(FILES apps/openssl/openssl.1 DESTINATION libexec/darling/usr/share/man/man1)

# empty dir
install(DIRECTORY DESTINATION libexec/darling/private/etc/ssl/certs)
install(
	FILES
		cert.pem
		openssl.cnf
		x509v3.cnf
	DESTINATION libexec/darling/private/etc/ssl
)
