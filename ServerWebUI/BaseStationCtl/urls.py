from django.conf.urls.defaults import patterns, include, url

# Uncomment the next two lines to enable the admin:
# from django.contrib import admin
# admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'BaseStationCtl.views.home', name='home'),
    # url(r'^BaseStationCtl/', include('BaseStationCtl.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    url(r'^info/', 'server.views.info'),
    url(r'^recentFreqUsed/', 'server.views.recentFreqUsed'),
    url(r'^css/(?P<path>.*)$', 'django.views.static.serve', {'document_root':'/root/graduate_demo/ServerWebUI/BaseStationCtl/css'}),
    url(r'^jquery/(?P<path>.*)$', 'django.views.static.serve', {'document_root':'/root/graduate_demo/ServerWebUI/BaseStationCtl/jquery'}),
    url(r'^jqplot/(?P<path>.*)$', 'django.views.static.serve', {'document_root':'/root/graduate_demo/ServerWebUI/BaseStationCtl/jqplot'}),
)
