/*jslint evil:true */
/*global DISQUS:false */
/**
 * Dynamic thread loader
 *
 * 
 *  * 
 * 
*/

(function (window) {
    var DISQUS = window.DISQUS;
    var jsonData, cookieMessages, session;

    // 
    if (!DISQUS || typeof DISQUS === 'function') {
        throw "DISQUS object is not initialized";
    }
    // 
    // json_data and default_json django template variables will close
    // and re-open javascript comment tags

    /* */ jsonData = {"reactions": [], "reactions_limit": 10, "ordered_highlighted": [], "posts": {"428465833": {"edited": false, "author_is_moderator": false, "from_request_user": null, "up_voted": false, "can_edit": false, "ip": "", "last_modified_date": null, "dislikes": 0, "raw_message": "HI, Very informative tutorial. I was wondering if profiling of other counters (like L1 CACHE hit/miss) is also possible by using oprofile in timer mode ?\n\nThanks, Marko", "has_replies": false, "vote": false, "votable": true, "last_modified_by": null, "real_date": "2012-02-03_05:20:20", "date": "1 year ago", "message": "<p>HI, Very informative tutorial. I was wondering if profiling of other counters (like L1 CACHE hit/miss) is also possible by using oprofile in timer mode ?</p>\n\n<p>Thanks, Marko</p>", "approved": true, "is_last_child": false, "author_is_founder": false, "can_reply": true, "likes": 0, "user_voted": null, "num_replies": 0, "down_voted": false, "is_first_child": false, "has_been_anonymized": false, "highlighted": false, "parent_post_id": null, "depth": 0, "points": 0, "user_key": "444abc49a3783065c306f9db7d5af33e", "author_is_creator": false, "email": "", "killed": false, "is_realtime": false}, "458531897": {"edited": false, "author_is_moderator": false, "from_request_user": false, "up_voted": false, "can_edit": false, "ip": "", "last_modified_date": null, "dislikes": 0, "raw_message": "Hi,\n\nThe timer mode is very simple, does not rely on PMU and unfortunately can't use any other counters. I'm not very happy about the PMU brokenness myself and using the timer is clearly a workaround. However the timer is reliable and allows to at least easily identify WHAT is slow in the program. This alone is already very useful. The advanced performance counters can help getting a better understanding WHY these parts of code are slow.", "has_replies": false, "vote": false, "votable": true, "last_modified_by": null, "real_date": "2012-03-07_03:34:18", "date": "11 months ago", "message": "<p>Hi,</p>\n\n<p>The timer mode is very simple, does not rely on PMU and unfortunately can't use any other counters. I'm not very happy about the PMU brokenness myself and using the timer is clearly a workaround. However the timer is reliable and allows to at least easily identify WHAT is slow in the program. This alone is already very useful. The advanced performance counters can help getting a better understanding WHY these parts of code are slow.</p>", "approved": true, "is_last_child": false, "author_is_founder": false, "can_reply": true, "likes": 0, "user_voted": null, "num_replies": 0, "down_voted": false, "is_first_child": false, "has_been_anonymized": false, "highlighted": false, "parent_post_id": null, "depth": 0, "points": 0, "user_key": "google-da94de3290d0e0e1c513eeb42dc324af", "author_is_creator": false, "email": "", "killed": false, "is_realtime": false}}, "ordered_posts": [428465833, 458531897], "realtime_enabled": false, "ready": true, "mediaembed": [], "has_more_reactions": false, "realtime_paused": true, "integration": {"receiver_url": "", "hide_user_votes": false, "reply_position": true, "disqus_logo": false}, "highlighted": {}, "reactions_start": 0, "media_url": "http://mediacdn.disqus.com/1361301849", "users": {"google-da94de3290d0e0e1c513eeb42dc324af": {"username": "google-da94de3290d0e0e1c513eeb42dc324af", "registered": true, "is_remote": true, "facebook": "", "verified": false, "about": "", "display_name": "Siarhei Siamashka", "url": "http://disqus.com/google-da94de3290d0e0e1c513eeb42dc324af/", "remote_id": "da94de3290d0e0e1c513eeb42dc324af", "blog": "", "points": 0, "avatar": "http://mediacdn.disqus.com/1361301849/images/noavatar32.png", "remote_domain": 6, "twitter": "", "remote_domain_name": "Google"}, "444abc49a3783065c306f9db7d5af33e": {"username": "Marko Panger", "registered": false, "is_remote": false, "facebook": "", "verified": false, "about": "", "display_name": "Marko Panger", "url": "http://disqus.com/guest/444abc49a3783065c306f9db7d5af33e/", "remote_id": null, "blog": "", "points": 0, "avatar": "http://mediacdn.disqus.com/1361301849/images/noavatar32.png", "remote_domain": "", "twitter": "", "remote_domain_name": ""}}, "user_unapproved": {}, "messagesx": {"count": 0, "unread": []}, "thread": {"voters_count": 0, "offset_posts": 0, "slug": "yet_another_oprofile_tutorial", "likes": 0, "num_pages": 1, "days_alive": 0, "moderate_none": false, "voters": {}, "total_posts": 2, "realtime_paused": true, "queued": false, "pagination_type": "append", "user_vote": null, "num_posts": 2, "closed": false, "per_page": 20, "id": 393962959, "killed": false, "moderate_all": false}, "forum": {"use_media": true, "avatar_size": 32, "apiKey": "f2Ah4r7VXt4Ve3ewc9UY7wX9aq0YpXfG46Uz3WevV62hOeZnC7FI6OKYwurJZ8ZS", "features": {}, "comment_max_words": 0, "mobile_theme_disabled": false, "is_early_adopter": false, "login_buttons_enabled": false, "streaming_realtime": false, "reply_position": true, "id": 808598, "default_avatar_url": "http://mediacdn.disqus.com/1361301849/images/noavatar32.png", "template": {"url": "http://mediacdn.disqus.com/1361301849/uploads/themes/dsq7884a9652e94555c70f96b6be63be216.js?255", "mobile": {"url": "http://mediacdn.disqus.com/1361301849/uploads/themes/mobile/theme.js?254", "css": "http://mediacdn.disqus.com/1361301849/uploads/themes/mobile/theme.css?254"}, "api": "1.1", "name": "Houdini", "css": "http://mediacdn.disqus.com/1361301849/uploads/themes/dsq7884a9652e94555c70f96b6be63be216.css?255"}, "max_depth": 10, "ranks_enabled": false, "lastUpdate": 1360229249, "linkbacks_enabled": false, "allow_anon_votes": true, "revert_new_login_flow": false, "stylesUrl": "http://mediacdn.disqus.com/uploads/styles/80/8598/ssvb.css", "show_avatar": true, "reactions_enabled": false, "disqus_auth_disabled": false, "name": "Siarhei&#39;s Blog", "language": "en", "mentions_enabled": true, "url": "ssvb", "allow_anon_post": true, "thread_votes_disabled": false, "hasCustomStyles": false, "moderate_all": false}, "settings": {"uploads_url": "http://media.disqus.com/uploads", "ssl_media_url": "https://securecdn.disqus.com/1361301849", "realtime_url": "http://rt.disqus.com/forums/realtime-cached.js", "facebook_app_id": "52254943976", "minify_js": true, "recaptcha_public_key": "6LdKMrwSAAAAAPPLVhQE9LPRW4LUSZb810_iaa8u", "read_only": false, "facebook_api_key": "52254943976", "juggler_url": "http://juggler.services.disqus.com", "debug": false, "disqus_url": "http://disqus.com", "media_url": "http://mediacdn.disqus.com/1361301849"}, "ranks": {}, "request": {"sort": "oldest", "is_authenticated": false, "user_type": "anon", "subscribe_on_post": 0, "missing_perm": null, "user_id": null, "remote_domain_name": "", "remote_domain": "", "is_verified": false, "profile_url": "", "username": "", "is_global_moderator": false, "sharing": {}, "timestamp": "2013-02-20_17:27:48", "is_moderator": false, "ordered_unapproved_posts": [], "unapproved_posts": {}, "forum": "ssvb", "is_initial_load": true, "display_username": "", "points": null, "has_email": false, "moderator_can_edit": false, "is_remote": false, "userkey": "", "page": 1}, "context": {"use_twitter_signin": false, "use_fb_connect": false, "show_reply": true, "sigma_chance": 10, "use_google_signin": false, "switches": {"listactivity_replies": true, "juggler_enabled": true, "next_realtime_indicators": true, "community_icon": true, "static_styles": true, "stats": true, "website_addons": true, "firehose_gnip_http": true, "discovery_next": true, "show_captcha_on_links": true, "next_dragdrop_nag": true, "firehose_gnip": true, "firehose_pubsub": true, "dark_jester": true, "limit_get_posts_days_30d": true, "juggler_thread_onReady": true, "disqus_trends": true, "discovery_next:top_placement": true, "upload_media": true, "shardpost:index": true, "filter_ads_by_country": true, "new_sort_paginator": true, "use_rs_paginator_5m": true, "firehose_push": true, "enable_link_affiliation": true, "limit_textdigger": true, "textdigger_crawler": true, "discovery_analytics": true, "discovery_next:truncate": true, "listactivity_replies_30d": true, "next_discard_low_rep": true, "mentions": true, "shardpost": true}, "forum_facebook_key": "", "use_yahoo": false, "subscribed": false, "active_gargoyle_switches": ["community_icon", "dark_jester", "discovery_analytics", "discovery_next", "discovery_next:top_placement", "discovery_next:truncate", "disqus_trends", "enable_link_affiliation", "filter_ads_by_country", "firehose_gnip", "firehose_gnip_http", "firehose_pubsub", "firehose_push", "juggler_enabled", "juggler_thread_onReady", "limit_get_posts_days_30d", "limit_textdigger", "listactivity_replies", "listactivity_replies_30d", "mentions", "new_sort_paginator", "next_discard_low_rep", "next_dragdrop_nag", "next_realtime_indicators", "shardpost", "shardpost:index", "show_captcha_on_links", "static_styles", "stats", "textdigger_crawler", "use_rs_paginator_5m", "website_addons"], "realtime_speed": 15000, "use_openid": false}}; /* */
    /* __extrajson__ */
    cookieMessages = {"user_created": null, "post_has_profile": null, "post_twitter": null, "post_not_approved": null}; session = {"url": null, "name": null, "email": null};

    DISQUS.jsonData = jsonData;
    DISQUS.jsonData.cookie_messages = cookieMessages;
    DISQUS.jsonData.session = session;

    if (DISQUS.useSSL) {
        DISQUS.useSSL(DISQUS.jsonData.settings);
    }

    // The mappings below are for backwards compatibility--before we port all the code that
    // accesses jsonData.settings to DISQUS.settings

    var mappings = {
        debug:                'disqus.debug',
        minify_js:            'disqus.minified',
        read_only:            'disqus.readonly',
        recaptcha_public_key: 'disqus.recaptcha.key',
        facebook_app_id:      'disqus.facebook.appId',
        facebook_api_key:     'disqus.facebook.apiKey'
    };

    var urlMappings = {
        disqus_url:    'disqus.urls.main',
        media_url:     'disqus.urls.media',
        ssl_media_url: 'disqus.urls.sslMedia',
        realtime_url:  'disqus.urls.realtime',
        uploads_url:   'disqus.urls.uploads'
    };

    if (DISQUS.jsonData.context.switches.realtime_setting_change) {
        urlMappings.realtimeHost = 'realtime.host';
        urlMappings.realtimePort = 'realtime.port';
    }
    for (key in mappings) {
        if (mappings.hasOwnProperty(key)) {
            DISQUS.settings.set(mappings[key], DISQUS.jsonData.settings[key]);
        }
    }

    for (key in urlMappings) {
        if (urlMappings.hasOwnProperty(key)) {
            DISQUS.jsonData.settings[key] = DISQUS.settings.get(urlMappings[key]);
        }
    }

    DISQUS.jsonData.context.csrf_token = '21bc467119200cb06806902fa8e2f5b0';

    DISQUS.jsonData.urls = {
        login: 'http://disqus.com/profile/login/',
        logout: 'http://disqus.com/logout/',
        upload_remove: 'http://ssvb.disqus.com/thread/yet_another_oprofile_tutorial/async_media_remove/',
        request_user_profile: 'http://disqus.com/AnonymousUser/',
        request_user_avatar: 'http://mediacdn.disqus.com/1361301849/images/noavatar92.png',
        verify_email: 'http://disqus.com/verify/',
        remote_settings: 'http://ssvb.disqus.com/_auth/embed/remote_settings/',
        edit_profile_window: 'http://disqus.com/embed/profile/edit/',
        embed_thread: 'http://ssvb.disqus.com/thread.js',
        embed_vote: 'http://ssvb.disqus.com/vote.js',
        embed_thread_vote: 'http://ssvb.disqus.com/thread_vote.js',
        embed_thread_share: 'http://ssvb.disqus.com/thread_share.js',
        embed_queueurl: 'http://ssvb.disqus.com/queueurl.js',
        embed_hidereaction: 'http://ssvb.disqus.com/hidereaction.js',
        embed_more_reactions: 'http://ssvb.disqus.com/more_reactions.js',
        embed_subscribe: 'http://ssvb.disqus.com/subscribe.js',
        embed_highlight: 'http://ssvb.disqus.com/highlight.js',
        embed_block: 'http://ssvb.disqus.com/block.js',
        update_moderate_all: 'http://ssvb.disqus.com/update_moderate_all.js',
        update_days_alive: 'http://ssvb.disqus.com/update_days_alive.js',
        show_user_votes: 'http://ssvb.disqus.com/show_user_votes.js',
        forum_view: 'http://ssvb.disqus.com/yet_another_oprofile_tutorial',
        cnn_saml_try: 'http://disqus.com/saml/cnn/try/',
        realtime: DISQUS.jsonData.settings.realtime_url,
        thread_view: 'http://ssvb.disqus.com/thread/yet_another_oprofile_tutorial/',
        twitter_connect: DISQUS.jsonData.settings.disqus_url + '/_ax/twitter/begin/',
        yahoo_connect: DISQUS.jsonData.settings.disqus_url + '/_ax/yahoo/begin/',
        openid_connect: DISQUS.jsonData.settings.disqus_url + '/_ax/openid/begin/',
        googleConnect: DISQUS.jsonData.settings.disqus_url + '/_ax/google/begin/',
        community: 'http://ssvb.disqus.com/community.html',
        admin: 'http://ssvb.disqus.com/admin/moderate/',
        moderate: 'http://ssvb.disqus.com/admin/moderate/',
        moderate_threads: 'http://ssvb.disqus.com/admin/moderate-threads/',
        settings: 'http://ssvb.disqus.com/admin/settings/',
        unmerged_profiles: 'http://disqus.com/embed/profile/unmerged_profiles/',
        juggler: DISQUS.jsonData.settings.juggler_url,

        channels: {
            def:      'http://disqus.com/default.html', /* default channel */
            auth:     'https://disqus.com/embed/login.html',
            tweetbox: 'http://disqus.com/forums/integrations/twitter/tweetbox.html?f=ssvb',
            edit:     'http://ssvb.disqus.com/embed/editcomment.html'
        }
    };


    // 
    //     
    DISQUS.jsonData.urls.channels.reply = 'http://mediacdn.disqus.com/1361301849/build/system/reply.html';
    DISQUS.jsonData.urls.channels.upload = 'http://mediacdn.disqus.com/1361301849/build/system/upload.html';
    DISQUS.jsonData.urls.channels.sso = 'http://mediacdn.disqus.com/1361301849/build/system/sso.html';
    DISQUS.jsonData.urls.channels.facebook = 'http://mediacdn.disqus.com/1361301849/build/system/facebook.html';
    //     
    // 
}(window));
